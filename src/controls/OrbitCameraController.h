//
// Created by Tobias on 10/2/2020.
//

#ifndef SD3D_ORBITCAMERACONTROLLER_H
#define SD3D_ORBITCAMERACONTROLLER_H

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(pop)

#include <GLFW/glfw3.h>

#include "../graphics/Camera.h"

class OrbitCameraController {
private:
	// split mouse vector in 2 directions
	// x -> theta
	// y -> phi

	glm::vec3 m_target{};
	float m_dist{};

	float m_yaw{-90.0f};
	float m_pitch{};

	glm::vec2 m_moveVec{};

	// TODO get from constructor
	const float m_deceleration{0.95f};
	const float m_speed{};

	Camera m_cam;

public:
	OrbitCameraController(Camera &&cam, const glm::vec3 &target, float dist, float speed = 5.0f, float deceleration = 0.95f) :
			m_target{target}, m_dist{dist}, m_cam{cam}, m_speed{speed}, m_deceleration{deceleration} {}

	void update(float deltaTime);

	[[nodiscard]] const Camera &cam() const { return m_cam; }

	void move(double dX, double dY);

};


#endif //SD3D_ORBITCAMERACONTROLLER_H
