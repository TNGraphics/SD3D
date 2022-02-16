//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_BUILTIN_SHADERS_H
#define SD3D_BUILTIN_SHADERS_H

#include "../ShaderHandle.hpp"

#include "shaders/color.frag.sprv.h"
#include "shaders/color.vert.sprv.h"
#include "shaders/error.frag.sprv.h"
#include "shaders/error.vert.sprv.h"
#include "shaders/lit.frag.sprv.h"
#include "shaders/lit.vert.sprv.h"

namespace sd3d::shaders {

[[maybe_unused]] constexpr ShaderHandle error_vertex_data() {
	return ShaderHandle {error_vert_data};
}

[[maybe_unused]] constexpr ShaderHandle error_fragment_data() {
	return ShaderHandle {error_frag_data};
}

[[maybe_unused]] constexpr ShaderHandle color_vertex_data() {
	return ShaderHandle {color_vert_data};
}

[[maybe_unused]] constexpr ShaderHandle color_fragment_data() {
	return ShaderHandle {color_frag_data};
}

[[maybe_unused]] constexpr ShaderHandle lit_vertex_data() {
	return ShaderHandle {lit_vert_data};
}

[[maybe_unused]] constexpr ShaderHandle lit_fragment_data() {
	return ShaderHandle {lit_frag_data};
}

} // namespace sd3d::shaders

#undef SHADER

#endif // SD3D_BUILTIN_SHADERS_H
