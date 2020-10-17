//
// Created by Tobias on 10/2/2020.
//

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>

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
