//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_TRANSFORMSHADER_H
#define SD3D_TRANSFORMSHADER_H

#include <string_view>
#include <optional>

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "Shader.hpp"

class TransformShader : public Shader {
public:
	struct PositionData {
		glm::mat4 model{};
		glm::mat4 view{};
		glm::mat4 projection{};
		std::optional<glm::mat3> normalMatrix{std::nullopt};
	};

private:
	sd3d::memory::shared_ssbo_t m_ssbo{};
	PositionData m_positionData;

protected:
	TransformShader(const char *vertSource, const char *fragSource);
	TransformShader(ShaderHandle vertex, ShaderHandle fragment);

public:
	TransformShader(std::string_view vertexPath, std::string_view fragmentPath);

	// TODO support partial updates
	void reset();
	void model(const glm::mat4 &model);
	void view(const glm::mat4 &view);
	void projection(const glm::mat4 &projection);
	void normal_mat(const glm::mat3 &normalMat);
	void apply_transform() const;
};

#endif // SD3D_TRANSFORMSHADER_H
