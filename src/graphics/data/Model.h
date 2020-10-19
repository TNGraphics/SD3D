//
// Created by Tobias on 10/14/2020.
//

#ifndef SD3D_MODEL_H
#define SD3D_MODEL_H

#include <string>
#include <vector>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "DataLayout.h"

class GlMesh;

struct aiNode;
struct aiScene;
struct aiMesh;

class Model {
public:
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 texCoords{};

		Vertex();
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	};

	static const DataLayout &vertex_layout();

private:
	// TODO handle textures in some way

	std::vector<GlMesh> m_meshes{};
	std::string m_directory{};

	Model() = default;

	void process_node(aiNode *node, const aiScene *scene);
	static GlMesh process_mesh(aiMesh *mesh, const aiScene *scene);

public:
	void draw() const;

	void clear();

	[[nodiscard]] size_t mesh_count() const;

	static Model from_path(const std::string &path);
};

#endif // SD3D_MODEL_H
