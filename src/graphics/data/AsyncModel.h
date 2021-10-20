//
// Created by Tobias on 10/15/2021.
//

#ifndef SD3D_ASYNCMODEL_H
#define SD3D_ASYNCMODEL_H

#include <filesystem>
#include <future>
#include <optional>
#include <string>
#include <vector>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "DataLayout.h"
#include "detail/AsyncAssimpNode.h"

class GlMesh;
class LitShader;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class AsyncModel {
public:
	enum class State { INITIAL, VALID, LOADING, INVALID };

private:
	using message_queue_t = sd3d::assimp::detail::AsyncAssimpNode::message_queue_t;

	std::future<std::optional<sd3d::assimp::detail::AsyncAssimpNode>> m_future;
	std::optional<sd3d::assimp::detail::AsyncAssimpNode> m_nodeTree{};
	std::string m_directory;
	std::string m_filename;
	std::unique_ptr<message_queue_t> m_messages;

	std::optional<std::string> m_currentlyLoadingNode{};

	State m_state{State::INITIAL};
	glm::mat4 m_cachedTransform;

public:
	AsyncModel() = default;
	explicit AsyncModel(const char *path,
						glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::string &path,
						glm::mat4 transformation = glm::mat4{1.0});
	explicit AsyncModel(const std::filesystem::path &path,
						glm::mat4 transformation = glm::mat4{1.0});
	AsyncModel(const AsyncModel &) = delete;
	AsyncModel &operator=(const AsyncModel &) = delete;
	AsyncModel(AsyncModel &&) noexcept = default;
	AsyncModel &operator=(AsyncModel &&) noexcept = default;

	void update_state();
	[[nodiscard]] State state() const;

	void draw(LitShader &);
	void draw();

	[[nodiscard]] const std::optional<std::string> &currently_loading_node() const;
	[[nodiscard]] const std::string &filename() const;

	[[nodiscard]] bool loading() const { return state() == State::LOADING; }

	// TODO queue the transformation if the model is not loaded yet
	void apply_transform(glm::mat4 transform);

	void clear();
};

#endif // SD3D_ASYNCMODEL_H
