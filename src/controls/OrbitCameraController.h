//
// Created by Tobias on 10/2/2020.
//

#ifndef SD3D_ORBITCAMERACONTROLLER_H
#define SD3D_ORBITCAMERACONTROLLER_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(pop)

#include "../graphics/Camera.h"

class OrbitCameraController {
public:
	struct OrbitCamSettings {
		glm::vec3 target{};

		float dist{};
		float minDist{};
		float maxDist{};
		float scrollSpeed{};

		float speed{};
		float deceleration{};

		OrbitCamSettings(const glm::vec3 &target, float dist, float speed, float deceleration = 0.05f) :
				OrbitCamSettings{target, dist, dist, dist, 0.0f, speed, deceleration} {}

		OrbitCamSettings(const glm::vec3 &target, float dist, float minDist, float maxDist, float scrollSpeed, float speed,
						 float deceleration = 0.05f) :
				target{target},
				dist{dist},
				minDist{minDist},
				maxDist{maxDist},
				scrollSpeed{scrollSpeed},
				speed{speed},
				deceleration{deceleration} {}
	};

private:
	// split mouse vector in 2 directions
	// x -> theta
	// y -> phi

	glm::vec3 m_target{};

	float m_dist{};
	const float m_minDist{};
	const float m_maxDist{};
	const float m_scrollSpeed{};

	float m_yaw{-90.0f};
	float m_pitch{};

	glm::vec2 m_moveVec{};

	// TODO get from constructor
	const float m_deceleration{0.05f};
	const float m_speed{};

	Camera m_cam;

public:
	OrbitCameraController(Camera &&cam, const OrbitCamSettings &settings) :
			m_target{settings.target},
			m_dist{settings.dist},
			m_minDist{settings.minDist},
			m_maxDist{settings.maxDist},
			m_scrollSpeed{settings.scrollSpeed},
			m_cam{cam},
			m_speed{settings.speed},
			m_deceleration{settings.deceleration} {}

	void update(float deltaTime);

	[[nodiscard]] const Camera &cam() const { return m_cam; }

	void rotate(double dX, double dY);

	void zoom(double dScroll);

};


#endif //SD3D_ORBITCAMERACONTROLLER_H
