//
// Created by Tobias on 10/16/2021.
//

#include "../memory/gl_memory_helpers.h"

#include "AsyncGlMesh.h"

#include "DataLayout.h"

namespace data = sd3d::data;

static void unbind_all_buffers() {
	// VAO always HAS to be first, because otherwise it may loose the buffers
	// stored inside
	glBindVertexArray(0);
	// EBO is not necessarily used by some GlMesh instances, doesn't hurt to
	// unbind it anyway
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AsyncGlMesh::add_texture(
	const AsyncGlMesh::TextureLoadInstruction &instruction) {
	GlMesh::add_texture(instruction.path, instruction.type, instruction.slot,
						instruction.settings);
}

AsyncGlMesh AsyncGlMesh::async_from_ai_mesh(aiMesh *mesh, const aiScene *scene,
											const std::string &texDir,
											glm::mat4 transform) {
	AsyncGlMesh glMesh{data::extract_vertices(mesh),
					   data::extract_indices(mesh), transform};
	if (mesh->mMaterialIndex >= 0) {
		glMesh.process_material(scene->mMaterials[mesh->mMaterialIndex],
								texDir);
	}
	return glMesh;
}

AsyncGlMesh::AsyncGlMesh(std::vector<data::Vertex> data,
						 std::vector<GLuint> indices, glm::mat4 transform) :
	GlMesh(true, transform),
	m_data{std::move(data)},
	m_indices{std::move(indices)} {}

void AsyncGlMesh::finalize() {
	// Load meshes into GPU
	m_vao = sd3d::memory::shared_vao_t{};
	m_vbo = sd3d::memory::shared_vbo_t{};
	m_ebo = sd3d::memory::shared_ebo_t{};

	m_drawCount = m_indices.size();
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(m_vao.name());

	sd3d::memory::fill_buffer(
		m_vbo, static_cast<GLsizeiptr>(m_data.size() * sizeof(data::Vertex)),
		m_data.data());
	sd3d::memory::fill_buffer(
		m_ebo, static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
		m_indices.data());

	vertex_layout().bind();

	unbind_all_buffers();

	// clear the data in RAM
	m_data.clear();
	m_indices.clear();

	// Load all textures
	for (const auto &instruction : m_texturesToLoad) {
		add_texture(instruction);
	}
	m_texturesToLoad.clear();

	if (m_textures.empty()) {
		// add white texture
		m_textures.push_back(Texture::empty_white());
	}

	m_initialized = true;
}
void AsyncGlMesh::process_material_textures_of_type(aiMaterial *mat,
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
		m_texturesToLoad.push_back(AsyncGlMesh::TextureLoadInstruction{
			.path = texDir + filename.C_Str(),
			.settings = defaultSettings,
			.slot = GL_TEXTURE0,
			.type = sd3d::assimp::from_assimp_type(type)});
	}
}
