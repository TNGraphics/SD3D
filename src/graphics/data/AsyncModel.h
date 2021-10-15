//
// Created by Tobias on 10/15/2021.
//

#ifndef SD3D_ASYNCMODEL_HPP
#define SD3D_ASYNCMODEL_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <future>

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

class AsyncModel {
public:
	enum State {
		Valid,
		Loading,
		Invalid
	};

private:
	std::future<sd3d::assimp::detail::AssimpNode> m_future;
	std::string m_dictionary;

	AsyncModel(AsyncModel &&) noexcept;

public:
	AsyncModel() = default;
	explicit AsyncModel(const char *path, glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::string &path, glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::filesystem::path &path, glm::mat4 transformation = glm::mat4{1.0});
	AsyncModel(const AsyncModel &) = default;
	AsyncModel &operator=(const AsyncModel &);
	AsyncModel &operator=(AsyncModel &&) noexcept;

	State state();

	void draw(LitShader &) const;
	void draw();

	void apply_transform(glm::mat4 transformation);

	void clear();
};

#endif // SD3D_ASYNCMODEL_HPP
