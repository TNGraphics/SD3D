//
// Created by Tobias on 10/26/2020.
//

#ifndef SD3D_COLORSHADER_H
#define SD3D_COLORSHADER_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "TransformShader.h"

class ColorShader : public TransformShader {
private:
	GLint m_colorLoc;

protected:
	ColorShader(const char *vertSource, const char *fragSource);
public:
	ColorShader();

	void set_color(const glm::vec3 &val) const;
};

#endif // SD3D_COLORSHADER_H
