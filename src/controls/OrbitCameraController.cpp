//
// Created by Tobias on 10/2/2020.
//

#define _USE_MATH_DEFINES

#include <cmath>
#include <algorithm>

#include <spdlog/spdlog.h>

#include "OrbitCameraController.h"

void OrbitCameraController::update(float deltaTime) {
	glm::vec3 dir{cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), sin(glm::radians(m_pitch)),
				  sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))};

	m_cam.set_pos(m_target - (dir * m_dist));
	m_cam.set_forward(dir);

	m_yaw += static_cast<float>(m_moveVec.x) * deltaTime * m_speed;
	m_pitch += static_cast<float>(m_moveVec.y) * deltaTime * m_speed;
	// choose value closest to 90/-90 using std::nextafter (from cmath)
	// otherwise the camera flips at 90 degrees
	m_pitch = std::clamp(m_pitch, std::nextafter(-90.0f, 0.0f), std::nextafter(90.0f, 0.0f));

	m_moveVec *= m_deceleration;
}

void OrbitCameraController::move(double dX, double dY) {
	// TODO maybe swap
	m_moveVec = glm::vec2{dX, -dY};
//	spdlog::info("Before: {},{}", m_moveVec.x, m_moveVec.y);
//	if(glm::length(m_moveVec)) {
//		m_moveVec = glm::normalize(m_moveVec);
//	}
//	spdlog::info("After: {},{}", m_moveVec.x, m_moveVec.y);
}
