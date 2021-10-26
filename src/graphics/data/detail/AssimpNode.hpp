//
// Created by Tobias on 10/28/2020.
//

#ifndef SD3D_ASSIMPNODE_H
#define SD3D_ASSIMPNODE_H

#include <vector>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

class GlMesh;
class LitShader;

struct aiScene;
struct aiNode;

namespace sd3d::assimp::detail {

class AssimpNode {
private:
	std::vector<GlMesh> m_meshes{};
	// TODO find better way than every node a vector
	std::vector<AssimpNode> m_nodes{};

	glm::mat4 m_transformation{};

	void process_node(aiNode *node, const aiScene *scene,
					  const std::string &directory, glm::mat4 transformation);

public:
	// TODO right now it cannot be filled with data anymore
	AssimpNode() = default;
	AssimpNode(const AssimpNode &) = default;
	AssimpNode(AssimpNode &&) noexcept ;
	AssimpNode &operator=(const AssimpNode &);
	AssimpNode &operator=(AssimpNode &&) noexcept;

	AssimpNode(aiNode *node, const aiScene *scene, const std::string &directory,
			   glm::mat4 transformation = glm::mat4{1.0});

	void draw(LitShader &) const;
	void draw() const;

	void apply_transform(glm::mat4 transform);

	void clear();
};

}

#endif // SD3D_ASSIMPNODE_H
