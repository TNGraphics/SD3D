//
// Created by Tobias on 10/2/2020.
//

#include <algorithm>
#include <cmath>

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(pop)

#include <spdlog/spdlog.h>

#include "OrbitCameraController.h"

void OrbitCameraController::update(float deltaTime) {
	glm::vec3 dir{cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
				  sin(glm::radians(m_pitch)),
				  sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))};

	m_cam.set_pos(m_target - (dir * m_dist));
	m_cam.set_forward(dir);

	// delta time isn't needed because mouse input is not coming every frame
	m_yaw += static_cast<float>(m_moveVec.x) * m_speed;
	m_pitch += static_cast<float>(m_moveVec.y) * m_speed;

	// otherwise the camera flips at 90 degrees
	m_pitch = std::clamp(m_pitch, std::nextafter(-90.0f, 0.0f),
						 std::nextafter(90.0f, 0.0f));
	m_moveVec *= std::max(0.0f, 1.0f - (m_deceleration * deltaTime));
}

void OrbitCameraController::rotate(double dX, double dY) {
	m_moveVec = glm::vec2{dX, -dY};
}

void OrbitCameraController::zoom(double dScroll) {
	m_dist -= static_cast<float>(dScroll) * m_scrollSpeed;
	m_dist = std::clamp(m_dist, m_minDist, m_maxDist);
}

void OrbitCameraController::settings_gui(bool &show, glm::vec3 &clearCol) {
	ImGui::SetNextWindowSize({-1, -1});
	ImGui::Begin("Camera Settings", &show);
	if(ImGui::SliderFloat("FOV", &m_fovTemp, 45.f, 120.f)) {
		cam().set_fov(m_fovTemp);
	}
	ImGui::SliderFloat("Move Speed", &m_speed, 0.1f, 1.f);
	ImGui::DragFloat("Deceleration", &m_deceleration, 0.1f, 0.5f, 35.f);
	if(ImGui::DragFloatRange2("Scroll Range", &m_minDist, &m_maxDist, 0.2f, 0.1f, 100.f)) {
		if(m_maxDist > m_minDist) {
			m_dist = std::clamp(m_dist, m_minDist, m_maxDist);
		}
	}
	ImGui::SliderFloat("Scroll Speed", &m_scrollSpeed, 0.5f, 5.f);
	if(ImGui::Button("Reset Settings")) {
		reset_settings();
	}
	ImGui::Separator();
	ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearCol));
	ImGui::End();
}
