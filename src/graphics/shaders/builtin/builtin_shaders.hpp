//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_BUILTIN_SHADERS_H
#define SD3D_BUILTIN_SHADERS_H

#include "shaders/color.frag.sprv.h"
#include "shaders/color.vert.sprv.h"
#include "shaders/error.frag.sprv.h"
#include "shaders/error.vert.sprv.h"
#include "shaders/lit.frag.sprv.h"
#include "shaders/lit.vert.sprv.h"

namespace sd3d::shaders {

struct ShaderHandle {
	const uint8_t *const bytes;
	const size_t length;

	constexpr ShaderHandle(const uint8_t *data, size_t length) :
		bytes{data}, length{length} {}
};

[[maybe_unused]] constexpr ShaderHandle error_vertex_data() {
	return {error_vert_data.data(), error_vert_data.size()};
}

[[maybe_unused]] constexpr const char *error_vertex_src() {
	constexpr const char *src = {
#include "src/error.vert"
	};
	return src;
}
[[maybe_unused]] constexpr const char *error_fragment_src() {
	constexpr const char *src = {
#include "src/error.frag"
	};
	return src;
}

[[maybe_unused]] constexpr const char *color_vertex_src() {
	constexpr const char *src = {
#include "src/color.vert"
	};
	return src;
}
[[maybe_unused]] constexpr const char *color_fragment_src() {
	constexpr const char *src = {
#include "src/color.frag"
	};
	return src;
}

[[maybe_unused]] constexpr const char *lit_vertex_src() {
	constexpr const char *src = {
#include "src/lit.vert"
	};
	return src;
}
[[maybe_unused]] constexpr const char *lit_fragment_src() {
	constexpr const char *src = {
#include "src/lit.frag"
	};
	return src;
}

} // namespace sd3d::shaders

#undef SHADER

#endif // SD3D_BUILTIN_SHADERS_H
