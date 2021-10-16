//
// Created by Tobias on 10/15/2021.
//

#ifndef SD3D_ASYNCMODEL_H
#define SD3D_ASYNCMODEL_H

#include <string>
#include <vector>
#include <filesystem>
#include <future>
#include <optional>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "detail/AsyncAssimpNode.h"
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
	enum class State {
		Valid,
		Loading,
		Invalid
	};

private:
	std::future<std::optional<sd3d::assimp::detail::AsyncAssimpNode>> m_future;
	std::optional<sd3d::assimp::detail::AsyncAssimpNode> m_nodeTree{};
	std::string m_directory;

	bool m_loadingFailed;
	bool m_finishedLoading;

	AsyncModel(AsyncModel &&) noexcept = default;

public:
	AsyncModel() = default;
	explicit AsyncModel(const char *path, glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::string &path, glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::filesystem::path &path, glm::mat4 transformation = glm::mat4{1.0});
	AsyncModel(const AsyncModel &) = delete;
	AsyncModel &operator=(const AsyncModel &) = delete;
	AsyncModel &operator=(AsyncModel &&) noexcept = default;

	State state();

	void draw(LitShader &);
	void draw();

	// TODO queue the transformation if the model is not loaded yet
	void apply_transform(glm::mat4 transformation);

	void clear();
};

#endif // SD3D_ASYNCMODEL_H
