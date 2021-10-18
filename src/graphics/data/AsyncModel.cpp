//
// Created by Tobias on 10/15/2021.
//

#include "AsyncModel.h"

#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "GlMesh.h"

AsyncModel::AsyncModel(const char *path, glm::mat4 transformation)
	: AsyncModel{std::filesystem::path{path}, transformation} {}

AsyncModel::AsyncModel(const std::string &path, glm::mat4 transformation)
	: AsyncModel{std::filesystem::path{path}, transformation} {}

AsyncModel::AsyncModel(const std::filesystem::path &path,
					   glm::mat4 transformation)
	: m_directory{path.parent_path().string() + '/'}, m_finishedLoading{false}, m_loadingFailed{false} {
	// OpenGL functions can only be called from the main thread
	// The lambda could capture a mutex or something, then data could be passed around
	m_future = std::async(std::launch::async, [&transformation](const std::string& directory, const auto &path) {
		Assimp::Importer importer;
		const auto *scene = importer.ReadFile(
			path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
			!scene->mRootNode) {
			spdlog::error("Assimp error: {}", importer.GetErrorString());
			return std::optional<sd3d::assimp::detail::AsyncAssimpNode>{};
		} else {
			auto ret = sd3d::assimp::detail::AsyncAssimpNode(scene->mRootNode, scene, directory, transformation);
			spdlog::debug("Finished loading {}", path.string());
			return std::optional{ret};
		}
	}, m_directory, path);
}

AsyncModel::State AsyncModel::state() {
	// Maybe split checking the state and returning the state
	if (m_loadingFailed) {
		return AsyncModel::State::Invalid;
	} else if (m_finishedLoading || !m_future.valid()) {
		return AsyncModel::State::Valid;
	} else {
		if (m_future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
			auto loadedModel = m_future.get();
			if (loadedModel) {
				m_finishedLoading = true;
				m_nodeTree = loadedModel;
				m_nodeTree->finalize();
				return AsyncModel::State::Valid;
			} else {
				m_loadingFailed = true;
				return AsyncModel::State::Invalid;
			}
		} else {
			return AsyncModel::State::Loading;
		}
	}
}

void AsyncModel::draw(LitShader &shader) {
	if (state() == AsyncModel::State::Valid && m_nodeTree) {
		m_nodeTree->draw(shader);
	}
}

void AsyncModel::draw() {
	if (state() == AsyncModel::State::Valid && m_nodeTree) {
		m_nodeTree->draw();
	}
}

void AsyncModel::apply_transform(glm::mat4 transformation) {}

void AsyncModel::clear() {
	if (m_nodeTree) {
		m_nodeTree->clear();
	}
}
