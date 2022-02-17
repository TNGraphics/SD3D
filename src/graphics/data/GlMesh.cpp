//
// Created by Tobias on 10/13/2020.
//

#include <spdlog/spdlog.h>

#include <assimp/scene.h>

#include <utility>

#include <gsl/gsl-lite.hpp>

#include "../memory/gl_memory_helpers.hpp"
#include "detail/assimp_helpers.hpp"

#include "../shaders/LitShader.hpp"
#include "DataLayout.hpp"

#include "GlMesh.hpp"

namespace mem = sd3d::memory;
namespace data = sd3d::data;

const DataLayout &GlMesh::vertex_layout() {
	static DataLayout d{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{2, DataLayout::GlType::FLOAT, GL_FALSE}};
	return d;
}

static void unbind_all_buffers() {
	// VAO always HAS to be first, because otherwise it may loose the buffers
	// stored inside
	glBindVertexArray(0);
	// EBO is not necessarily used by some GlMesh instances, doesn't hurt to
	// unbind it anyway
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GlMesh::GlMesh(const DataLayout &dataLayout, const float *data, GLuint amount,
			   glm::mat4 transform) :
	m_vao{},
	m_vbo{},
	m_ebo{nullptr}, // otherwise it gets one generated
	m_usesEbo{false},
	m_drawCount{amount},
	m_initialized{true},
	m_modelMatrix{transform},
	m_normalMatrix{glm::transpose(glm::inverse(transform))} {
	glBindVertexArray(m_vao.name());

	mem::fill_buffer(m_vbo, amount, data);

	dataLayout.bind();

	unbind_all_buffers();
}

GlMesh::GlMesh(const std::vector<data::Vertex> &data,
			   const std::vector<GLuint> &indices, glm::mat4 transform) :
	m_vao{},
	m_vbo{},
	m_ebo{},
	m_usesEbo{true},
	m_drawCount{static_cast<GLuint>(indices.size())},
	m_initialized{true},
	m_modelMatrix{transform},
	m_normalMatrix{glm::transpose(glm::inverse(transform))} {
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(m_vao.name());

	sd3d::memory::fill_buffer(m_vbo, data.size() * sizeof(data::Vertex), data.data());
	sd3d::memory::fill_buffer(m_ebo, indices.size() * sizeof(unsigned int),
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
	m_textures{std::move(other.m_textures)},
	m_modelMatrix{other.m_modelMatrix},
	m_normalMatrix{other.m_normalMatrix} {
	other.m_vao.reset();
	other.m_drawCount = 0;
	other.m_vbo.reset();
	other.m_ebo.reset();
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
	other.m_modelMatrix = glm::mat4{1.0};
	other.m_normalMatrix = glm::mat4{1.0};
}

GlMesh::GlMesh(bool useEbo, glm::mat4 transform) :
	m_ebo{nullptr},
	m_vao{nullptr},
	m_vbo{nullptr},
	m_usesEbo{useEbo},
	m_drawCount{0},
	m_modelMatrix{transform},
	m_normalMatrix{glm::transpose(glm::inverse(transform))} {}

GlMesh &GlMesh::operator=(GlMesh &&other) noexcept {
	m_vao = std::move(other.m_vao);
	m_drawCount = other.m_drawCount;
	m_vbo = std::move(other.m_vbo);
	m_ebo = std::move(other.m_ebo);
	m_usesEbo = other.m_usesEbo;
	m_initialized = other.m_initialized;
	m_textures = std::move(other.m_textures);
	m_modelMatrix = other.m_modelMatrix;
	m_normalMatrix = other.m_normalMatrix;

	other.m_vao.reset();
	other.m_drawCount = 0;
	other.m_vbo.reset();
	other.m_ebo.reset();
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
	other.m_modelMatrix = glm::mat4{1.0};
	other.m_normalMatrix = glm::mat4{1.0};
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

void GlMesh::draw(LitShader &shader) const {
	if (!m_initialized) return;

	placeholder_tex().bind_to_num(0);

	shader.model(m_modelMatrix);
	shader.normal_mat(m_normalMatrix);

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

	shader.apply_transform();

	draw_mesh();

	for (unsigned int i = 1; const auto &tex : m_textures) {
		Texture::unbind_num(i);
		++i;
	}
	Texture::unbind_num(0);
	Texture::reset();
}

void GlMesh::draw_mesh() const {
	glBindVertexArray(m_vao.name());
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

GlMesh GlMesh::from_ai_mesh(aiMesh *mesh, const aiScene *scene,
							const std::string &texDir, glm::mat4 transform) {
	GlMesh glMesh{data::extract_vertices(mesh), data::extract_indices(mesh), transform};
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
		// TODO this doesn't support multiple textures of the same type
		add_texture(texDir + filename.C_Str(),
					sd3d::assimp::from_assimp_type(type), defaultSettings);
	}
}

void GlMesh::set_transform(glm::mat4 transform) {
	m_modelMatrix = transform;
	m_normalMatrix = glm::transpose(glm::inverse(transform));
}
