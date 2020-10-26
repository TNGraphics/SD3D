//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_LITSHADER_H
#define SD3D_LITSHADER_H

#include <array>
#include <utility>

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "ColorShader.h"

// TODO structurize
namespace light {
using distance_t = int;

struct Attenuation {
	float constant;
	float linear;
	float quadratic;

	constexpr Attenuation(float c, float l, float q) :
		constant{c},
		linear{l},
		quadratic{q} {}
	constexpr Attenuation(float l, float q) :
		constant{1.f},
		linear{l},
		quadratic{q} {}
	constexpr Attenuation() : constant{}, linear{}, quadratic{} {}
};

static constexpr std::array<std::pair<distance_t, Attenuation>, 12>
	g_attenuationLookup{{{7, {0.7f, 1.8f}},
						 {13, {0.35f, 0.44f}},
						 {20, {0.22f, 0.20f}},
						 {32, {0.14f, 0.07f}},
						 {50, {0.09f, 0.032f}},
						 {65, {0.07f, 0.017f}},
						 {100, {0.045f, 0.0075f}},
						 {160, {0.027f, 0.0028f}},
						 {200, {0.022f, 0.0019f}},
						 {325, {0.014f, 0.0007f}},
						 {600, {0.007f, 0.0002f}},
						 {3250, {0.0014f, 0.000007f}}}};

constexpr Attenuation from_distance(distance_t distance) {
	for (unsigned int i = 0; i < g_attenuationLookup.size(); ++i) {
		if (distance < g_attenuationLookup[i].first) {
			// its either this or the element before
			if (i <= 0 ||
				std::abs(distance - g_attenuationLookup[i - 1].first) >
					std::abs(distance - g_attenuationLookup[i].first)) {
				return g_attenuationLookup[i].second;
			} else {
				return g_attenuationLookup[i - 1].second;
			}
		}
	}
	return g_attenuationLookup.back().second;
}

} // namespace light

class LitShader : public ColorShader {
private:
	struct AttenuationLoc {
		GLint constant;
		GLint linear;
		GLint quadratic;
	};

private:
	static constexpr unsigned int s_plightCount = 4;
	using plight_attribs_t = std::array<GLint, s_plightCount>;

	void load_light_locs();
	plight_attribs_t m_plightPositions{};
	plight_attribs_t m_plightDiffuse{};
	plight_attribs_t m_plightSpecular{};
	plight_attribs_t m_plightColor{};
	std::array<AttenuationLoc, s_plightCount> m_attenuations{};

	GLint m_camPosLoc;

	GLint m_dirDirectionLoc;
	GLint m_dirColorLoc;
	GLint m_dirAmbientLoc;
	GLint m_dirDiffuseLoc;
	GLint m_dirSpecularLoc;

//	vec3 direction;
//
//	vec3 color;
//	float ambient;
//	float diffuse;
//	float specular;

public:
	LitShader();

	void set_cam_pos(const glm::vec3 &pos) const;

	void set_plight_position(unsigned int index, const glm::vec3 &val) const;
	void set_plight_diffuse(unsigned int index, float val) const;
	void set_plight_specular(unsigned int index, float val) const;
	void set_plight_color(unsigned int index, const glm::vec3  &val) const;
	void set_plight_attenuation(unsigned int index, light::Attenuation val) const;

	void set_dir_light_direction(const glm::vec3 &val) const;
	void set_dir_light_color(const glm::vec3 &val) const;
	void set_dir_light_ambient(float val) const;
	void set_dir_light_diffuse(float val) const;
	void set_dir_light_specular(float val) const;
};

#endif // SD3D_LITSHADER_H
