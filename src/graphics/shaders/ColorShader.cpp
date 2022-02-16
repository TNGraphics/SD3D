//
// Created by Tobias on 10/26/2020.
//

#include "ColorShader.hpp"

#include "builtin/builtin_shaders.hpp"

ColorShader::ColorShader(const char *vertSource, const char *fragSource) :
	TransformShader(vertSource, fragSource),
	m_colorLoc{get_uniform_loc("color")} {}

ColorShader::ColorShader(ShaderHandle vertex, ShaderHandle fragment) :
	TransformShader(vertex, fragment),
	m_colorLoc{get_uniform_loc("color")}{}

void ColorShader::set_color(const glm::vec3 &val) const {
	set(m_colorLoc, val);
}

ColorShader::ColorShader() :
	ColorShader{sd3d::shaders::color_vertex_data(), sd3d::shaders::color_fragment_data()} {}
