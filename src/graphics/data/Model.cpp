//
// Created by Tobias on 10/14/2020.
//

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <spdlog/spdlog.h>

#include "Model.h"

#include "GlMesh.h"

const DataLayout &Model::vertex_layout() {
	static DataLayout d{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{2, DataLayout::GlType::FLOAT, GL_FALSE}};
	return d;
}

void Model::draw() const {
	for (const auto &mesh : m_meshes) {
		mesh.draw();
	}
}

void Model::process_node(aiNode *node, const aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		auto *mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(process_mesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], scene);
	}
}

// TODO use scene
GlMesh Model::process_mesh(aiMesh *mesh, const aiScene *) {
	std::vector<Vertex> vertices{};

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
				 : glm::vec3{std::sqrt(1.f)}),
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

Model::Vertex::Vertex(glm::vec3 position, glm::vec3 normal,
					  glm::vec2 texCoords) :
	position{position},
	normal{normal},
	texCoords{texCoords} {}

Model::Vertex::Vertex() : position{}, normal{}, texCoords{} {}
