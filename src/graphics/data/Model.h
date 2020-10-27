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

#include "DataLayout.h"

class GlMesh;
class Shader;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class Model {
private:
	std::vector<GlMesh> m_meshes{};
	// for later for textures ;)
	std::string m_directory{};

	bool m_isValid{};

	Model(Model &&) noexcept;

	void process_node(aiNode *node, const aiScene *scene);

public:
	Model() = default;
	explicit Model(const char *path);
	explicit Model(const std::string &path);
	explicit Model(const std::filesystem::path &path);
	Model(const Model &) = default;
	Model &operator=(const Model &);
	Model &operator=(Model &&) noexcept;

	explicit operator bool() const;

	void draw() const;
	void draw(Shader &) const;

	void clear();

	[[nodiscard]] size_t mesh_count() const;
};

#endif // SD3D_MODEL_H
