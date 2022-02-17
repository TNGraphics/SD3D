//
// Created by Tobias on 10/26/2020.
//

#include "../memory/gl_memory_helpers.hpp"

#include "ColorShader.hpp"

#include "builtin/builtin_shaders.hpp"

ColorShader::ColorShader(const char *vertSource, const char *fragSource) :
	TransformShader(vertSource, fragSource) {}

ColorShader::ColorShader(ShaderHandle vertex, ShaderHandle fragment) :
	TransformShader(vertex, fragment){}

void ColorShader::set_color(const glm::vec3 &val) const {
	GLenum type = sd3d::memory::fill_ssbo(m_ssbo, &val);
	glBindBufferBase(type, 1, m_ssbo.name());
}

ColorShader::ColorShader() :
	ColorShader{sd3d::shaders::color_vertex_data(), sd3d::shaders::color_fragment_data()} {}
