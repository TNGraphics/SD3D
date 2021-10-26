//
// Created by Tobias on 10/28/2020.
//

#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include "../GlMesh.hpp"

#include "AssimpNode.hpp"

namespace sd3d::assimp::detail {

AssimpNode::AssimpNode(aiNode *node, const aiScene *scene,
					   const std::string &directory, glm::mat4 transformation) {
	process_node(node, scene, directory, transformation);
}

AssimpNode::AssimpNode(AssimpNode &&other) noexcept :
	m_meshes{std::move(other.m_meshes)},
	m_nodes{std::move(other.m_nodes)},
	m_transformation{other.m_transformation} {}

AssimpNode &AssimpNode::operator=(const AssimpNode &other) {
	if (&other != this) {
		m_meshes = other.m_meshes;
		m_nodes = other.m_nodes;
		m_transformation = other.m_transformation;
	}
	return *this;
}
AssimpNode &AssimpNode::operator=(AssimpNode &&other) noexcept {
	if (&other != this) {
		m_meshes = std::move(other.m_meshes);
		m_nodes = std::move(other.m_nodes);
		m_transformation = other.m_transformation;
	}
	return *this;
}

void AssimpNode::process_node(aiNode *node, const aiScene *scene,
							  const std::string &directory,
							  glm::mat4 transformation) {
	spdlog::debug("Loading node [{}]", node->mName.C_Str());
	auto aT{node->mTransformation};
	glm::mat4 nodeTransformation{aT.a1, aT.b1, aT.c1, aT.d1, aT.a2, aT.b2,
								aT.c2, aT.d2, aT.a3, aT.b3, aT.c3, aT.d3,
								aT.a4, aT.b4, aT.c4, aT.d4};
	m_transformation = nodeTransformation;

	transformation *= nodeTransformation;
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		m_meshes.push_back(
			GlMesh::from_ai_mesh(scene->mMeshes[node->mMeshes[i]], scene,
								 directory, transformation));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		m_nodes.emplace_back(node->mChildren[i], scene, directory,
							 transformation);
	}
}

void AssimpNode::draw(LitShader &shader) const {
	for (const auto &mesh : m_meshes) {
		mesh.draw(shader);
	}
	for (const auto &node : m_nodes) {
		node.draw(shader);
	}
}

void AssimpNode::draw() const {
	for (const auto &mesh : m_meshes) {
		mesh.draw();
	}
	for (const auto &node : m_nodes) {
		node.draw();
	}
}

void AssimpNode::apply_transform(glm::mat4 transform) {
	auto mul{transform * m_transformation};
	for(auto &mesh : m_meshes) {
		mesh.set_transform(mul);
	}
	for(auto &node : m_nodes) {
		node.apply_transform(mul);
	}
}

void AssimpNode::clear() {
	m_meshes.clear();
	m_nodes.clear();
}

} // namespace sd3d::assimp::detail
