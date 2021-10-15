//
// Created by Tobias on 10/14/2020.
//

#ifndef SD3D_MODEL_H
#define SD3D_MODEL_H

#include <string>
#include <vector>
#include <filesystem>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "detail/AssimpNode.h"
#include "DataLayout.h"

class GlMesh;
class LitShader;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class Model {
private:
	sd3d::assimp::detail::AssimpNode m_nodeTree;
	// for later for textures ;)
	std::string m_directory{};

	bool m_isValid{};

	Model(Model &&) noexcept;

//	void process_node(aiNode *node, const aiScene *scene);

public:
	Model() = default;
	explicit Model(const char *path, glm::mat4 transformation = glm::mat4{1.0});
	explicit Model(const std::string &path, glm::mat4 transformation = glm::mat4{1.0});
	explicit Model(const std::filesystem::path &path, glm::mat4 transformation = glm::mat4{1.0});
	Model(const Model &) = default;
	Model &operator=(const Model &);
	Model &operator=(Model &&) noexcept;

	explicit operator bool() const;

	void draw(LitShader &) const;
	void draw() const;

	void apply_transform(glm::mat4 transformation);

	void clear();
};

#endif // SD3D_MODEL_H
