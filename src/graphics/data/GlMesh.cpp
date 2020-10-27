//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <assimp/scene.h>

#include <utility>

#include <gsl/gsl-lite.hpp>

#include "detail/assimp_helpers.h"

#include "../shaders/Shader.h"
#include "DataLayout.h"

#include "GlMesh.h"

namespace mem = sd3d::memory;

const DataLayout &GlMesh::vertex_layout() {
	static DataLayout d{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{2, DataLayout::GlType::FLOAT, GL_FALSE}};
	return d;
}

static void unbind_all_buffers() {
	glBindVertexArray(0);
	// EBO is not necessarily used by some GlMesh instances, doesn't hurt to
	// unbind it anyway
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// TODO probably should be in some header
enum class BufferType {
	// TODO add more
	VBO,
	EBO
};

static constexpr GLenum gl_buffer_type(BufferType type) {
	switch (type) {
	case BufferType::VBO:
		return GL_ARRAY_BUFFER;
	case BufferType::EBO:
		return GL_ELEMENT_ARRAY_BUFFER;
	}
	return GL_INVALID_ENUM;
}

// FIXME don't like void * but might be the only option

/// Bind and fill a buffer with the provided data
/// \tparam type The type of buffer to bind and fill
/// \param buffer The actual name of the buffer to bind and fill
/// \param size The size of the data you want to fill the buffer with
/// \param data The pointer to the data you want to fill the buffer with
template<BufferType type>
static void fill_buffer(GLint buffer, GLsizeiptr size, const void *data) {
	glBindBuffer(gl_buffer_type(type), buffer);
	// TODO other modes that GL_STATIC_DRAW
	glBufferData(gl_buffer_type(type), size, data, GL_STATIC_DRAW);
}

GlMesh::GlMesh(const DataLayout &dataLayout, const float *data, GLuint amount) :
	m_vao{mem::create_vao()},
	m_vbo{mem::create_vbo()},
	m_ebo{nullptr},
	m_usesEbo{false},
	m_drawCount{amount},
	m_initialized{true} {
	glBindVertexArray(*m_vao);

	fill_buffer<BufferType::VBO>(*m_vbo, amount, data);

	dataLayout.bind();

	unbind_all_buffers();
}

GlMesh::GlMesh(const std::vector<Vertex> &data,
			   const std::vector<GLuint> &indices) :
	m_vao{mem::create_vao()},
	m_vbo{mem::create_vbo()},
	m_ebo{mem::create_ebo()},
	m_usesEbo{true},
	m_drawCount{static_cast<GLuint>(indices.size())},
	m_initialized{true} {
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(*m_vao);

	fill_buffer<BufferType::VBO>(*m_vbo, data.size() * sizeof(Vertex),
								 data.data());
	fill_buffer<BufferType::EBO>(*m_ebo, indices.size() * sizeof(unsigned int),
								 indices.data());

	vertex_layout().bind();

	unbind_all_buffers();
}

GlMesh::GlMesh(GlMesh &&other) noexcept :
	m_vao{std::move(other.m_vao)},
	m_drawCount{other.m_drawCount},
	m_vbo{std::move(other.m_vbo)},
	m_ebo{std::move(other.m_ebo)},
	m_usesEbo{other.m_usesEbo},
	m_initialized{other.m_initialized},
	m_textures{std::move(other.m_textures)} {
	other.m_vao.reset();
	other.m_drawCount = 0;
	other.m_vbo.reset();
	other.m_ebo.reset();
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
}

GlMesh &GlMesh::operator=(GlMesh &&other) noexcept {
	m_vao = std::move(other.m_vao);
	m_drawCount = other.m_drawCount;
	m_vbo = std::move(other.m_vbo);
	m_ebo = std::move(other.m_ebo);
	m_usesEbo = other.m_usesEbo;
	m_initialized = other.m_initialized;
	m_textures = std::move(other.m_textures);

	other.m_vao.reset();
	other.m_drawCount = 0;
	other.m_vbo.reset();
	other.m_ebo.reset();
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
	return *this;
}

GlMesh &GlMesh::operator=(const GlMesh &other) {
	if (this != &other) {
		m_vao = other.m_vao;
		m_drawCount = other.m_drawCount;
		m_vbo = other.m_vbo;
		m_ebo = other.m_ebo;
		m_usesEbo = other.m_usesEbo;
		m_initialized = other.m_initialized;
		m_textures = other.m_textures;
	}
	return *this;
}

void GlMesh::draw() const {
	if (!m_initialized) return;
	// bind all textures
	for (const auto &tex : m_textures) {
		tex.bind();
	}

	draw_mesh();

	// unbind all textures
	for (const auto &tex : m_textures) {
		tex.unbind();
	}
	Texture::reset();
}

void GlMesh::draw(Shader &shader) const {
	if (!m_initialized) return;

	placeholder_tex().bind_to_num(0);

	int diffuseNr = 0;
	int specularNr = 0;
	for (unsigned int i = 1; const auto &tex : m_textures) {
		std::string property{};
		if (tex.get_type() == Texture::Type::DIFFUSE) {
			property = "material.texture_diffuse" + std::to_string(++diffuseNr);
		} else if (tex.get_type() == Texture::Type::SPECULAR) {
			property =
				"material.texture_specular" + std::to_string(++specularNr);
		}
		shader.set_int(property.c_str(), gsl::narrow<int>(i));
		// bind the texture to that slot
		tex.bind_to_num(i);
		++i;
	}

	draw_mesh();

	for (unsigned int i = 1; const auto &tex : m_textures) {
		Texture::unbind_num(i);
		++i;
	}
	Texture::unbind_num(0);
	Texture::reset();
}

void GlMesh::draw_mesh() const {
	glBindVertexArray(*m_vao);
	if (m_usesEbo) {
		glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	}
	glBindVertexArray(0);
}

void GlMesh::add_texture(const char *path, Texture::Type type, GLenum slot,
						 const Texture::Settings &settings) {
	m_textures.emplace_back(path, settings, slot, type);
}

void GlMesh::add_texture(std::string_view path, Texture::Type type, GLenum slot,
						 const Texture::Settings &settings) {
	m_textures.emplace_back(path, settings, slot, type);
}

void GlMesh::add_texture(const char *path, Texture::Type type,
						 const Texture::Settings &settings) {
	add_texture(path, type, GL_TEXTURE0, settings);
}

void GlMesh::add_texture(std::string_view path, Texture::Type type,
						 const Texture::Settings &settings) {
	add_texture(path, type, GL_TEXTURE0, settings);
}

void GlMesh::finish_setup() {
	if (m_textures.empty()) {
		// add white texture
		m_textures.push_back(Texture::empty_white());
	}
}

const Texture &GlMesh::placeholder_tex() {
	static Texture tex{Texture::empty_black()};
	return tex;
}

static std::vector<unsigned int> extract_indices(const aiMesh *mesh);

static std::vector<GlMesh::Vertex> extract_vertices(const aiMesh *mesh);

GlMesh GlMesh::from_ai_mesh(aiMesh *mesh, const aiScene *scene,
							const std::string &texDir) {
	GlMesh glMesh{extract_vertices(mesh), extract_indices(mesh)};
	if (mesh->mMaterialIndex >= 0) {
		glMesh.process_material(scene->mMaterials[mesh->mMaterialIndex],
								texDir);
	}
	glMesh.finish_setup();
	return glMesh;
}

void GlMesh::process_material(aiMaterial *mat, const std::string &texDir) {
	process_material_textures_of_type(mat, aiTextureType_DIFFUSE, texDir);
	process_material_textures_of_type(mat, aiTextureType_SPECULAR, texDir);
}

void GlMesh::process_material_textures_of_type(aiMaterial *mat,
											   aiTextureType type,
											   const std::string &texDir) {
	static constexpr Texture::Settings defaultSettings{
		.wrapS = GL_REPEAT,
		.wrapT = GL_REPEAT,
		.minFilter = GL_LINEAR_MIPMAP_LINEAR,
		.magFilter = GL_LINEAR};
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString filename;
		mat->GetTexture(type, i, &filename);
		add_texture(texDir + filename.C_Str(),
					sd3d::assimp::from_assimp_type(type), defaultSettings);
	}
}

std::vector<unsigned int> extract_indices(const aiMesh *mesh) {
	std::vector<unsigned int> indices{};
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return indices;
}

std::vector<GlMesh::Vertex> extract_vertices(const aiMesh *mesh) {
	std::vector<GlMesh::Vertex> vertices{};

	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		vertices.emplace_back(
			(mesh->HasPositions()
				 ? glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y,
							 mesh->mVertices[i].z}
				 : glm::vec3{0}),
			(mesh->HasNormals()
				 ? glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y,
							 mesh->mNormals[i].z}
				 : glm::vec3{0.0f, 1.0f, 0.0f}),
			(mesh->HasTextureCoords(0) ? glm::vec2{mesh->mTextureCoords[0][i].x,
												   mesh->mTextureCoords[0][i].y}
									   : glm::vec2{0}));
	}
	return vertices;
}

// region Vertex
GlMesh::Vertex::Vertex(glm::vec3 position, glm::vec3 normal,
					   glm::vec2 texCoords) :
	position{position},
	normal{normal},
	texCoords{texCoords} {}

GlMesh::Vertex::Vertex() : position{}, normal{}, texCoords{} {}
// endregion Vertex
