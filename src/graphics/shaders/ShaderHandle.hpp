//
// Created by Tobias on 2/16/2022.
//

#ifndef SD3D_SHADERHANDLE_HPP
#define SD3D_SHADERHANDLE_HPP

#include <array>

struct ShaderHandle {
	const uint8_t *const bytes;
	const size_t length;

	template<size_t N>
	consteval explicit ShaderHandle(const std::array<uint8_t, N> &data) :
		bytes{data.data()}, length{data.size()} {}
};

#endif // SD3D_SHADERHANDLE_HPP
