//
// Created by Tobias on 10/14/2020.
//

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <spdlog/spdlog.h>

#include "Model.h"

#include "GlMesh.h"

void Model::draw() const {
	for (const auto &mesh : m_meshes) {
		mesh.draw();
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
GlMesh Model::process_mesh(aiMesh *mesh, const aiScene *) {
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
	// TODO process materials
	return GlMesh::from_data(vertices, indices);
}

Model Model::from_path(const std::string &path) {
	Assimp::Importer importer;
	const auto *scene =
		importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		spdlog::error("Assimp error: {}", importer.GetErrorString());
		return {};
	}
	Model m{};
	m.m_directory = path.substr(0, path.find_last_of('/'));
	m.process_node(scene->mRootNode, scene);
	return m;
}

size_t Model::mesh_count() const {
	return m_meshes.size();
}

void Model::clear() {
	for (auto &mesh : m_meshes) {
		mesh.release_data();
	}
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
