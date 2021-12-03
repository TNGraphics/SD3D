//
// Created by Tobias on 12/3/2021.
//

#include "Transform.hpp"

void Transform::calculate_transformations() {
	if (!m_enabled || !m_dirty) {
		return;
	}
	m_transformation = glm::translate(glm::mat4_cast(m_rotation), m_position);

	m_dirty = false;
}

void Transform::set_active_state(bool newState) {
	m_enabled = newState;
}

void Transform::enable() {
	set_active_state(true);
}

void Transform::disable() {
	set_active_state(false);
}

void Transform::toggle_active() {
	m_enabled ^= true;
}

Transform::Transform(glm::vec3 position, glm::quat rotation) :
	m_position{position},
	m_rotation{rotation} {}

Transform::Transform(glm::vec3 position, glm::vec3 rotationAxis, float angle) :
	Transform{position, glm::angleAxis(angle, rotationAxis)} {}

Transform::Transform(glm::vec3 position) :
	Transform{position, glm::quat{}} {}

Transform::Transform(glm::quat rotation) : Transform{glm::vec3{}, rotation} {}

Transform::Transform(glm::vec3 rotationAxis, float angle) :
	Transform{glm::angleAxis(angle, rotationAxis)} {}

void Transform::rotate(const glm::vec3 &axis, float angle) {
	m_dirty = true;
	glm::rotate(m_rotation, angle, axis);
}

void Transform::rotate(const glm::quat &delta) {
	rotate(glm::axis(delta), glm::angle(delta));
}

void Transform::set_rotation(const glm::vec3 &axis, float angle) {
	m_dirty = true;
	m_rotation = glm::angleAxis(angle, axis);
}

void Transform::set_rotation(const glm::quat &newRotation) {
	m_dirty = true;
	m_rotation = newRotation;
}

void Transform::position(const glm::vec3 &newPosition) {
	m_dirty = true;
	m_position = newPosition;
}

void Transform::move(const glm::vec3 &delta) {
	m_dirty = true;
	m_dirty = true;
	m_position += delta;
}

void Transform::reset() {
	m_dirty = true;
	m_position = glm::vec3{};
	m_rotation = glm::quat{};
}

void Transform::set(glm::vec3 position, glm::quat rotation) {
	m_dirty = true;
	m_position = position;
	m_rotation = rotation;
}

void Transform::set(glm::vec3 position, glm::vec3 rotationAxis, float angle) {
	set(position, glm::angleAxis(angle, rotationAxis));
}

void Transform::set(glm::vec3 rotationAxis, float angle) {
	set(glm::vec3{}, rotationAxis, angle);
}
