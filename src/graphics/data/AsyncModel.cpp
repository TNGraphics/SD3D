//
// Created by Tobias on 10/15/2021.
//

#include "AsyncModel.hpp"

#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

AsyncModel::AsyncModel(const char *path, glm::mat4 transformation) :
	AsyncModel{std::filesystem::path{path}, transformation} {}

AsyncModel::AsyncModel(const std::string &path, glm::mat4 transformation) :
	AsyncModel{std::filesystem::path{path}, transformation} {}

AsyncModel::AsyncModel(const std::filesystem::path &path,
					   glm::mat4 transformation) :
	m_directory{path.parent_path().string() + '/'},
	m_cachedTransform{transformation},
	m_messages{std::make_unique<message_queue_t>()},
	m_filename{path.filename().string()} {
	m_future = std::async(
		std::launch::async,
		[&transformation](message_queue_t &msgs, const std::string &directory,
						  const auto &path) {
			Assimp::Importer importer;

			const auto *scene =
				importer.ReadFile(path.string().c_str(),
								  aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
				!scene->mRootNode) {
				spdlog::error("Assimp error: {}", importer.GetErrorString());
				return std::optional<sd3d::assimp::detail::AsyncAssimpNode>{};
			} else {
				auto ret = sd3d::assimp::detail::AsyncAssimpNode(
					msgs, scene->mRootNode, scene, directory, transformation);
				spdlog::debug("Finished loading {}", path.string());
				return std::optional{ret};
			}
		},
		std::ref(*m_messages), m_directory, path);
}

AsyncModel::State AsyncModel::state() const {
	return m_state;
}

void AsyncModel::update_state() {
	// TODO make it possible to abort the loading -> e.g. to close the window
	// TODO also update loading state from a SPSC thing or something like that
	if (m_state == State::LOADING || m_state == State::INITIAL) {
		if (m_future.valid()) {
			if (m_future.wait_for(std::chrono::nanoseconds(0)) ==
				std::future_status::ready) {
				auto loadedModel = m_future.get();
				if (loadedModel) {
					m_nodeTree = loadedModel;
					m_nodeTree->finalize();
					m_nodeTree->apply_transform(m_cachedTransform);
					m_currentlyLoadingNode = std::nullopt;
					m_state = AsyncModel::State::VALID;
				} else {
					m_state = AsyncModel::State::INVALID;
				}
			} else {
				if (m_messages) {
					std::unique_ptr<
						sd3d::assimp::detail::AsyncAssimpNode::LoadMessage>
						msg{};

					while (m_messages->try_pop(msg));

					if (msg) {
						m_currentlyLoadingNode = msg->nodeName;
					}
				}
				m_state = AsyncModel::State::LOADING;
			}
		}
	}
}

void AsyncModel::draw(LitShader &shader) {
	if (state() == AsyncModel::State::VALID && m_nodeTree) {
		m_nodeTree->draw(shader);
	}
}

void AsyncModel::draw() {
	if (state() == AsyncModel::State::VALID && m_nodeTree) {
		m_nodeTree->draw();
	}
}

void AsyncModel::apply_transform(glm::mat4 transform) {
	// TODO apply transformation after load
	if (!m_nodeTree) {
		// Cache transformation
		m_cachedTransform = transform;
	} else {
		// Directly apply
		m_nodeTree->apply_transform(transform);
	}
}

void AsyncModel::clear() {
	if (m_nodeTree) {
		// TODO clear other stuff
		m_nodeTree->clear();
	}
}

const std::optional<std::string> &AsyncModel::currently_loading_node() const {
	return m_currentlyLoadingNode;
}
const std::string &AsyncModel::filename() const {
	return m_filename;
}
