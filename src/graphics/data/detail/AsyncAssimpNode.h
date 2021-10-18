//
// Created by Tobias on 10/16/2021.
//

#ifndef SD3D_ASYNCASSIMPNODE_H
#define SD3D_ASYNCASSIMPNODE_H

#include <vector>

#include <spsc_queue.hpp>

#include "../AsyncGlMesh.h"

namespace sd3d::assimp::detail {

class AsyncAssimpNode {
public:
	struct LoadMessage {
		std::string nodeName;

		LoadMessage() = default;
		explicit LoadMessage(std::string nodeName);
	};
	using message_queue_t = deaod::spsc_queue<sd3d::assimp::detail::AsyncAssimpNode::LoadMessage, 8>;

private:
	std::vector<AsyncGlMesh> m_meshes{};
	std::vector<AsyncAssimpNode> m_nodes{};

	glm::mat4 m_transformation{};

	void process_node(message_queue_t &messageQueue, aiNode *node, const aiScene *scene,
					  const std::string &directory, glm::mat4 transformation);

	bool m_initialized{false};

public:
	AsyncAssimpNode() = default;
	AsyncAssimpNode(const AsyncAssimpNode &) = default;
	AsyncAssimpNode(AsyncAssimpNode &&) = default;
	AsyncAssimpNode &operator=(const AsyncAssimpNode &) = default;
	AsyncAssimpNode &operator=(AsyncAssimpNode &&) = default;

	AsyncAssimpNode(message_queue_t &messageQueue, aiNode *node, const aiScene *scene,
					const std::string &directory,
					glm::mat4 transformation = glm::mat4{1.0});

	void draw(LitShader &) const;
	void draw() const;

	void apply_transform(glm::mat4 transform);

	void clear();

	void finalize();
};
}

#endif // SD3D_ASYNCASSIMPNODE_H
