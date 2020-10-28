//
// Created by Tobias on 10/20/2020.
//

#ifndef SD3D_GL_MEMORY_H
#define SD3D_GL_MEMORY_H

#include <memory>
#include <concepts>
#include <utility>

#include <GLFW/glfw3.h>

namespace sd3d::memory {

namespace detail {

	using buffer_ptr_t = std::shared_ptr<GLuint>;
	using weak_buffer_ptr_t = std::weak_ptr<GLuint>;

	template<typename T>
	concept Creator = requires(T t) {
		// has to be callable
		{t()};
		// has to return a buffer_ptr_t
		std::same_as<decltype(t()), buffer_ptr_t>;
	};

	template<Creator creator_t>
	class Buffer {
	private:
		buffer_ptr_t m_buffer;

	public:
		Buffer() : m_buffer{creator_t()()} {}
		explicit Buffer(buffer_ptr_t bufferPtr) : m_buffer{std::move(bufferPtr)} {}
		Buffer(const Buffer &) = default;
		Buffer(Buffer &&) noexcept = default;
		Buffer &operator=(const Buffer &) = default;
		Buffer &operator=(Buffer &&) noexcept = default;
		// todo maybe accept assignment with nullptr_t ?

		// create a new gl object
		// This might delete the old object if no other object is pointing to it
		void create_new() { m_buffer = creator_t()(); }
		// Reset the buffer so it points to nowhere (nullptr)
		// The gpu object might be deleted, if no other buffer object points to it
		void reset() { m_buffer.reset(); }

		// TODO maybe implicit conversion to GLuint
		// TODO maybe operator* which does the same as name()

		// Get the name (id) of the object this is pointing to
		[[nodiscard]] GLuint name() const { return *m_buffer; }
		// Get a new weak_ptr that points to the underlying shared_ptr
		[[nodiscard]] weak_buffer_ptr_t weak() const { return m_buffer; }
	};

	// shared_ptrs don't have the deleter as type info, unique_ptrs do
	// sad, but it is what it is, this makes the different buffer types
	// as typedefs of shared_ptr<GLuint> not typesafe :(

	struct VboCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct VaoCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct EboCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct TexCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct ProgCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct FboCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};

	struct RboCreator {
		[[nodiscard]] buffer_ptr_t operator()();
	};
}

// This is also not perfectly typesafe, because this works:
// 		shared_tex_t tex;
// 		shared_vbo_t vbo{tex.weak().lock()};
// It still doesn't cause leaks, because the correct allocator and deleter are
// still used, the type names might just be abused, but oh well :^)

using shared_vbo_t = detail::Buffer<detail::VboCreator>;

using shared_vao_t = detail::Buffer<detail::VaoCreator>;

using shared_ebo_t = detail::Buffer<detail::EboCreator>;

using shared_tex_t = detail::Buffer<detail::TexCreator>;
using weak_tex_t = std::weak_ptr<GLuint>;

using shared_prog_t = detail::Buffer<detail::ProgCreator>;

using shared_fbo_t = detail::Buffer<detail::FboCreator>;

using shared_rbo_t = detail::Buffer<detail::RboCreator>;

} // namespace sd3d::memory

#endif // SD3D_GL_MEMORY_H
