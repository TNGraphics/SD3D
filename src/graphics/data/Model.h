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
private:
	// TODO handle textures in some way

	std::vector<GlMesh> m_meshes{};
	// for later for textures ;)
	std::string m_directory{};

	Model(Model &&) noexcept;

	void process_node(aiNode *node, const aiScene *scene);
	static GlMesh process_mesh(aiMesh *mesh, const aiScene *scene);

public:
	Model() = default;
	Model(const Model &) = default;
	Model &operator=(const Model &);
	Model &operator=(Model &&) noexcept;

	void draw() const;

	void clear();

	[[nodiscard]] size_t mesh_count() const;

	static Model from_path(const std::string &path);
};

#endif // SD3D_MODEL_H
