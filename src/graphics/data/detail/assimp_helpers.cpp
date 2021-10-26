//
// Created by Tobias on 10/16/2021.
//

#include "assimp_helpers.hpp"

namespace sd3d::data {

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) :
	position{position},
	normal{normal},
	texCoords{texCoords} {}

Vertex::Vertex() : position{}, normal{}, texCoords{} {}

std::vector<Vertex> extract_vertices(const aiMesh *mesh) {
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
				 : glm::vec3{0.0f, 1.0f, 0.0f}),
			(mesh->HasTextureCoords(0) ? glm::vec2{mesh->mTextureCoords[0][i].x,
												   mesh->mTextureCoords[0][i].y}
									   : glm::vec2{0}));
	}
	return vertices;
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

}
