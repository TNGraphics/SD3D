//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_BUILTIN_SHADERS_H
#define SD3D_BUILTIN_SHADERS_H

namespace sd3d::shaders {

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
