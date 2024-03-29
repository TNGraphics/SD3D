//
// Created by Tobias on 10/16/2021.
//

#include "AsyncAssimpNode.h"

#include <spdlog/spdlog.h>

namespace sd3d::assimp::detail {

AsyncAssimpNode::AsyncAssimpNode(aiNode *node, const aiScene *scene,
								 const std::string &directory,
								 glm::mat4 transformation) :
	m_initialized{false} {
	process_node(node, scene, directory, transformation);
}

void AsyncAssimpNode::process_node(aiNode *node, const aiScene *scene,
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
			AsyncGlMesh::async_from_ai_mesh(scene->mMeshes[node->mMeshes[i]],
											scene, directory, transformation));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		m_nodes.emplace_back(node->mChildren[i], scene, directory,
							 transformation);
	}
}

void AsyncAssimpNode::draw(LitShader &shader) const {
	if (!m_initialized) {
		return;
	}
	for (const auto &mesh : m_meshes) {
		mesh.draw(shader);
	}
	for (const auto &node : m_nodes) {
		node.draw(shader);
	}
}

void AsyncAssimpNode::draw() const {
	if (!m_initialized) {
		return;
	}
	for (const auto &mesh : m_meshes) {
		mesh.draw();
	}
	for (const auto &node : m_nodes) {
		node.draw();
	}
}

void AsyncAssimpNode::apply_transform(glm::mat4 transform) {
	auto mul{transform * m_transformation};
	for (auto &mesh : m_meshes) {
		mesh.set_transform(mul);
	}
	for (auto &node : m_nodes) {
		node.apply_transform(mul);
	}
}

void AsyncAssimpNode::clear() {
	m_meshes.clear();
	m_nodes.clear();
}

void AsyncAssimpNode::finalize() {
	for (auto &mesh : m_meshes) {
		mesh.finalize();
	}
	for (auto &node : m_nodes) {
		node.finalize();
	}
	m_initialized = true;
}

}
