//
// Created by Tobias on 10/28/2020.
//

#ifndef SD3D_GL_MEMORY_HELPERS_H
#define SD3D_GL_MEMORY_HELPERS_H

/// *****************************************
/// *                WARNING                *
/// *                                       *
/// *                                       *
/// *                                       *
/// *       Only include in cpp files       *
/// *         !!!!!!!!!!!!!!!!!!!!!         *
/// *                                       *
/// *                                       *
/// *                                       *
/// *****************************************

#include <concepts>

#include <glad/glad.h>

#include "gl_memory.h"

namespace sd3d::memory {

namespace detail {
template<typename T>
struct BufferInfo {
	static constexpr GLenum gl_type() { return T::not_implemented(); };
};

// TODO more types

template<>
struct BufferInfo<shared_vbo_t> {
	static constexpr GLenum gl_type() { return GL_ARRAY_BUFFER; }
};

template<>
struct BufferInfo<shared_ebo_t> {
	static constexpr GLenum gl_type() { return GL_ELEMENT_ARRAY_BUFFER; }
};

template<typename T>
concept FillableBuffer = requires {
	std::same_as<T, shared_vbo_t> || std::same_as<T, shared_ebo_t>;
};

} // namespace detail

/// Bind and fill a buffer with the provided data
/// \param buffer The sd3d::memory::Buffer to bind and fill
/// \param size The size of the data you want to fill the buffer with
/// \param data The pointer to the data you want to fill the buffer with
template<detail::FillableBuffer T>
void fill_buffer(T buffer, GLsizeiptr size, const void *data) {
	glBindBuffer(detail::BufferInfo<T>::gl_type(), buffer.name());
	// TODO other modes that GL_STATIC_DRAW
	glBufferData(detail::BufferInfo<T>::gl_type(), size, data, GL_STATIC_DRAW);
}

} // namespace sd3d::memory

#endif // SD3D_GL_MEMORY_HELPERS_H
