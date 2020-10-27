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

void Model::draw() const {
	for (const auto &mesh : m_meshes) {
		mesh.draw();
	}
}

void Model::draw(Shader &shader) const {
	for (const auto &mesh : m_meshes) {
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
		m_meshes.push_back(GlMesh::from_ai_mesh(mesh, scene, m_directory));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], scene);
	}
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

Model::Model(const char *path) : Model{std::filesystem::path{path}} {}

Model::Model(const std::string &path) : Model{std::filesystem::path{path}} {}

Model::Model(const std::filesystem::path &path) :
	m_directory{path.parent_path().string() + '/'} {
	Assimp::Importer importer;
	// FIXME don't like path.string().c_str()
	//  -> path has c_str() but that doesn't work for some reason
	const auto *scene =
		importer.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		spdlog::error("Assimp error: {}", importer.GetErrorString());
		m_directory.clear();
		m_isValid = false;
	} else {
		process_node(scene->mRootNode, scene);
		spdlog::debug("Finished loading {}", path.string());
		m_isValid = true;
	}
}

Model::operator bool() const {
	return m_isValid;
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
