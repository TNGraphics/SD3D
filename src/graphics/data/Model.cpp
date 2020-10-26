//
// Created by Tobias on 10/14/2020.
//

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <filesystem>

#include <spdlog/spdlog.h>

#include "Model.h"
#include "Texture.h"

#include "GlMesh.h"

static constexpr Texture::Type from_assimp_type(aiTextureType type);

void Model::draw() const {
	for (const auto &mesh : m_meshes) {
		mesh.draw();
	}
}

void Model::draw(Shader &shader) const {
	for(const auto &mesh : m_meshes) {
		mesh.draw(shader);
	}
}

void Model::process_node(aiNode *node, const aiScene *scene) {
	// Some models don't load correctly, probably because there are some
	// properties I don't read, maybe something like scale and position of a
	// mesh
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		// TODO use node->mTransformation
		auto *mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(process_mesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], scene);
	}
}

// TODO use scene
// TODO move to GlMesh
GlMesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
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
	// We are only dealing with triangles so the size will likely be num faces *
	// 3
	std::vector<unsigned int> indices{};
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	auto glMesh{GlMesh::from_data(vertices, indices)};
	if (mesh->mMaterialIndex >= 0) {
		process_material(scene->mMaterials[mesh->mMaterialIndex], glMesh);
	}
	glMesh.finish_setup();
	return glMesh;
}

void Model::process_material(aiMaterial *mat, GlMesh &mesh) {
	process_material_textures_of_type(mat, aiTextureType_DIFFUSE, mesh);
	process_material_textures_of_type(mat, aiTextureType_SPECULAR, mesh);
}

void Model::process_material_textures_of_type(aiMaterial *mat,
											  aiTextureType type,
											  GlMesh &mesh) {
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString filename;
		mat->GetTexture(type, i, &filename);
		mesh.add_texture(m_directory + filename.C_Str(), from_assimp_type(type),
						 Texture::Settings{.wrapS = GL_REPEAT,
										   .wrapT = GL_REPEAT,
										   .minFilter = GL_LINEAR_MIPMAP_LINEAR,
										   .magFilter = GL_LINEAR});
	}
}

Model Model::from_path(const std::string &path) {
	// TODO multi-threaded model loading
	Assimp::Importer importer;
	const auto *scene =
		importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		spdlog::error("Assimp error: {}", importer.GetErrorString());
		return {};
	}
	Model m{};
	auto p = std::filesystem::path(path);
	p.remove_filename();
	m.m_directory = p.string();
	m.process_node(scene->mRootNode, scene);
	spdlog::debug("Finished loading {}", path);
	return m;
}

size_t Model::mesh_count() const {
	return m_meshes.size();
}

void Model::clear() {
	m_meshes.clear();
}

Model &Model::operator=(Model &&other) noexcept {
	clear();
	// move data
	m_meshes = std::move(other.m_meshes);
	m_directory = std::move(other.m_directory);
	return *this;
}

Model::Model(Model &&other) noexcept :
	m_meshes{std::move(other.m_meshes)},
	m_directory{std::move(other.m_directory)} {}

Model &Model::operator=(const Model &other) {
	if (this != &other) {
		m_meshes = other.m_meshes;
		m_directory = other.m_directory;
	}
	return *this;
}

constexpr Texture::Type from_assimp_type(aiTextureType type) {
	switch (type) {
	case aiTextureType_DIFFUSE:
		return Texture::Type::DIFFUSE;
	case aiTextureType_SPECULAR:
		return Texture::Type::SPECULAR;
		// TODO handle more types
	case aiTextureType_AMBIENT:
	case aiTextureType_EMISSIVE:
	case aiTextureType_HEIGHT:
	case aiTextureType_NORMALS:
	case aiTextureType_SHININESS:
	case aiTextureType_OPACITY:
	case aiTextureType_DISPLACEMENT:
	case aiTextureType_LIGHTMAP:
	case aiTextureType_REFLECTION:
	case aiTextureType_BASE_COLOR:
	case aiTextureType_NORMAL_CAMERA:
	case aiTextureType_EMISSION_COLOR:
	case aiTextureType_METALNESS:
	case aiTextureType_DIFFUSE_ROUGHNESS:
	case aiTextureType_AMBIENT_OCCLUSION:
	case aiTextureType_UNKNOWN:
	case _aiTextureType_Force32Bit:
	case aiTextureType_NONE:
		return Texture::Type::DIFFUSE;
	}
	return Texture::Type::DIFFUSE;
}
