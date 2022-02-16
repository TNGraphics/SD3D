//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_TRANSFORMSHADER_H
#define SD3D_TRANSFORMSHADER_H

#include <string_view>

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "Shader.hpp"

class TransformShader : public Shader {
private:
	GLuint m_positionBlock{};
	GLint m_modelLoc{};
	GLint m_viewLoc{};
	GLint m_projLoc{};
	GLint m_normMatLoc{};
	void init_locations();

protected:
	TransformShader(const char *vertSource, const char *fragSource);
	TransformShader(ShaderHandle vertex, ShaderHandle fragment);

public:
	TransformShader(std::string_view vertexPath, std::string_view fragmentPath);

	void model(const glm::mat4 &modelMat) const;
	void view(const glm::mat4 &viewMat) const;
	void projection(const glm::mat4 &projMat) const;
	void normal_mat(const glm::mat3 &normMat) const;
};

#endif // SD3D_TRANSFORMSHADER_H
