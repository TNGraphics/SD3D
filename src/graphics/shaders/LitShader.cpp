//
// Created by Tobias on 10/21/2020.
//

#include "LitShader.hpp"

#include "builtin/builtin_shaders.hpp"

LitShader::LitShader() :
	ColorShader{sd3d::shaders::lit_vertex_data(),
				sd3d::shaders::lit_fragment_data()},
	m_camPosLoc{get_uniform_loc("camPos")},
	m_dirDirectionLoc{get_uniform_loc("dirLight.direction")},
	m_dirColorLoc{get_uniform_loc("dirLight.color")},
	m_dirAmbientLoc{get_uniform_loc("dirLight.ambient")},
	m_dirDiffuseLoc{get_uniform_loc("dirLight.diffuse")},
	m_dirSpecularLoc{get_uniform_loc("dirLight.specular")} {
	load_light_locs();
}

void LitShader::set_cam_pos(const glm::vec3 &pos) const {
	set(m_camPosLoc, pos);
}

void LitShader::load_light_locs() {
	for (int i = 0; i < s_plightCount; ++i) {
		auto numStr = std::to_string(i);
		m_plightPositions.at(i) =
			get_uniform_loc(("pointLights[" + numStr + "].position").c_str());
		m_plightDiffuse.at(i) =
			get_uniform_loc(("pointLights[" + numStr + "].diffuse").c_str());
		m_plightSpecular.at(i) =
			get_uniform_loc(("pointLights[" + numStr + "].specular").c_str());
		m_plightColor.at(i) =
			get_uniform_loc(("pointLights[" + numStr + "].color").c_str());

		m_attenuations.at(i) = {
			get_uniform_loc(("pointLights[" + numStr + "].constant").c_str()),
			get_uniform_loc(("pointLights[" + numStr + "].linear").c_str()),
			get_uniform_loc(("pointLights[" + numStr + "].quadratic").c_str())};
	}
}

void LitShader::set_plight(unsigned int index,
						   const light::PointLight &light) const {
	set_plight_position(index, light.position);
	set_plight_color(index, light.color);
	set_plight_diffuse(index, light.diffuse);
	set_plight_specular(index, light.specular);
}

void LitShader::set_plights(
	const std::array<light::PointLight, s_plightCount> &lights) const {
	for (unsigned int i = 0; i < s_plightCount; ++i) {
		set_plight(i, lights.at(i));
	}
}

void LitShader::set_plight_position(unsigned int index,
									const glm::vec3 &val) const {
	set(m_plightPositions.at(index), val);
}

void LitShader::set_plight_diffuse(unsigned int index, float val) const {
	set(m_plightDiffuse.at(index), val);
}

void LitShader::set_plight_specular(unsigned int index, float val) const {
	set(m_plightSpecular.at(index), val);
}

void LitShader::set_plight_color(unsigned int index,
								 const glm::vec3 &val) const {
	set(m_plightColor.at(index), val);
}

void LitShader::set_plight_attenuation(unsigned int index,
									   light::Attenuation val) const {
	auto locs = m_attenuations.at(index);
	set(locs.constant, val.constant);
	set(locs.linear, val.linear);
	set(locs.quadratic, val.quadratic);
}

void LitShader::set_dir_light_direction(const glm::vec3 &val) const {
	set(m_dirDirectionLoc, val);
}

void LitShader::set_dir_light_color(const glm::vec3 &val) const {
	set(m_dirColorLoc, val);
}

void LitShader::set_dir_light_ambient(float val) const {
	set(m_dirAmbientLoc, val);
}

void LitShader::set_dir_light_diffuse(float val) const {
	set(m_dirDiffuseLoc, val);
}

void LitShader::set_dir_light_specular(float val) const {
	set(m_dirSpecularLoc, val);
}

void LitShader::set_dir_light(const light::DirLight &light) const {
	set_dir_light_direction(light.direction);
	set_dir_light_color(light.color);
	set_dir_light_ambient(light.ambient);
	set_dir_light_diffuse(light.diffuse);
	set_dir_light_specular(light.specular);
}
