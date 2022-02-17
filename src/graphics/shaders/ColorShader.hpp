//
// Created by Tobias on 10/26/2020.
//

#ifndef SD3D_COLORSHADER_H
#define SD3D_COLORSHADER_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "TransformShader.hpp"

class ColorShader : public TransformShader {
private:
	sd3d::memory::shared_ssbo_t m_ssbo{};

protected:
	ColorShader(const char *vertSource, const char *fragSource);
	ColorShader(ShaderHandle vertex, ShaderHandle fragment);
public:
	ColorShader();

	void set_color(const glm::vec3 &val) const;
};

#endif // SD3D_COLORSHADER_H
