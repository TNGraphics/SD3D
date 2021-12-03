//
// Created by Tobias on 12/3/2021.
//

#ifndef SD3D_TRANSFORM_HPP
#define SD3D_TRANSFORM_HPP

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#pragma warning(pop)

class Transform {
// Rotation
// Position
// Parent
// Children?
// Name?

// Call to propagate Position downwards in the tree
private:
	bool m_enabled{};
	glm::vec3 m_position{};
	glm::quat m_rotation{};
	// TODO: attached Object or similar

	bool m_dirty{true};

	// Update transformation if something else is updated
	glm::mat4 m_transformation{};

public:
	Transform() = default;
	Transform(glm::vec3 position, glm::quat rotation);
	Transform(glm::vec3 position, glm::vec3 rotationAxis, float angle);
	Transform(glm::vec3 rotationAxis, float angle);
	explicit Transform(glm::vec3 position);
	explicit Transform(glm::quat rotation);

	void rotate(const glm::vec3 &axis, float angle);
	void rotate(const glm::quat &delta);

	void set_rotation(const glm::vec3 &axis, float angle);
	void set_rotation(const glm::quat &newRotation);

	void position(const glm::vec3 &newPosition);
	void move(const glm::vec3 &delta);

	void enable();
	void disable();
	void set_active_state(bool newState);
	void toggle_active();

	void reset();
	void set(glm::vec3 position, glm::quat rotation);
	void set(glm::vec3 position, glm::vec3 rotationAxis, float angle);
	void set(glm::vec3 rotationAxis, float angle);

	void calculate_transformations();
};

#endif // SD3D_TRANSFORM_HPP
