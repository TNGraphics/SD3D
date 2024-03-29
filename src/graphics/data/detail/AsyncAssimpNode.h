//
// Created by Tobias on 10/16/2021.
//

#ifndef SD3D_ASYNCASSIMPNODE_H
#define SD3D_ASYNCASSIMPNODE_H

#include <vector>

#include "../AsyncGlMesh.h"

namespace sd3d::assimp::detail {

class AsyncAssimpNode {
private:
	std::vector<AsyncGlMesh> m_meshes{};
	std::vector<AsyncAssimpNode> m_nodes{};

	glm::mat4 m_transformation{};

	void process_node(aiNode *node, const aiScene *scene,
					  const std::string &directory, glm::mat4 transformation);

	bool m_initialized;

public:
	AsyncAssimpNode() = default;
	AsyncAssimpNode(const AsyncAssimpNode &) = default;
	AsyncAssimpNode(AsyncAssimpNode &&) = default;
	AsyncAssimpNode &operator=(const AsyncAssimpNode &) = default;
	AsyncAssimpNode &operator=(AsyncAssimpNode &&) = default;

	AsyncAssimpNode(aiNode *node, const aiScene *scene,
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
