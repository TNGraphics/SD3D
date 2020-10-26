//
// Created by Tobias on 9/28/2020.
//

#ifndef SD3D_CAMERA_H
#define SD3D_CAMERA_H

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(pop)

#include <imgui.h>

class Camera {
private:
	static constexpr glm::vec3 s_worldUp = glm::vec3{0, 1, 0};

private:
	glm::vec3 m_pos{};
	glm::vec3 m_forward{};
	glm::vec3 m_right{};
	glm::vec3 m_up{};

	double m_fov{};
	double m_aspect{};

public:
	Camera(double fov, const glm::vec2 &aspect, glm::vec3 pos = glm::vec3{},
		   glm::vec3 forward = glm::vec3{0, 0, 1},
		   glm::vec3 up = glm::vec3{0, 1, 0}) :
		Camera{fov, aspect.x / aspect.y, pos, forward, up} {}

	Camera(double fov, double aspect, glm::vec3 pos = glm::vec3{},
		   glm::vec3 forward = glm::vec3{0, 0, 1},
		   glm::vec3 up = glm::vec3{0, 1, 0}) :
		m_pos{pos},
		m_forward{forward},
		m_up{up},
		m_right{glm::cross(m_forward, s_worldUp)},
		m_fov{fov},
		m_aspect{aspect} {}

	[[maybe_unused]] [[nodiscard]] const glm::vec3 &get_pos() const {
		return m_pos;
	}

	[[maybe_unused]] [[nodiscard]] const glm::vec3 &get_forward() const {
		return m_forward;
	}

	[[maybe_unused]] void set_pos(const glm::vec3 &pos) { m_pos = pos; }

	[[maybe_unused]] void move(const glm::vec3 &amount) { m_pos += amount; }

	[[maybe_unused]] void move(const glm::vec3 &axis, float amount) {
		move(axis * amount);
	}

	[[maybe_unused]] [[nodiscard]] glm::mat4 view() const {
		return glm::lookAt(m_pos, m_forward, m_up);
	}

	[[maybe_unused]] [[nodiscard]] glm::mat4 projection() const {
		return glm::perspective(glm::radians(m_fov), m_aspect, 0.1, 100.0);
	}

	[[maybe_unused]] void set_forward(const glm::vec3 &f) {
		m_forward = glm::normalize(f);
		m_right = glm::normalize(glm::cross(m_forward, s_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_forward));
	}

	[[maybe_unused]] void set_aspect(double aspect) {
		m_aspect = aspect;
	}

	[[nodiscard]] double aspect() const { return m_aspect; }

	[[maybe_unused]] [[nodiscard]] double fov() const { return m_fov; }

	[[maybe_unused]] void set_fov(double fov) { m_fov = fov; }
};

#endif // SD3D_CAMERA_H
