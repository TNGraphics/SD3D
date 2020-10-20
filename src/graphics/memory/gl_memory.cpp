//
// Created by Tobias on 10/20/2020.
//
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "gl_memory.h"

namespace sd3d::memory {

[[maybe_unused]] shared_vbo_t create_vbo() {
	auto vbo = shared_vbo_t(new GLuint{0}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting VBO {}", *ptr);
#endif
		glDeleteBuffers(1, ptr);
		delete ptr;
	});
	glGenBuffers(1, vbo.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating VBO {}", *vbo);
#endif
	return vbo;
}

[[maybe_unused]] shared_vao_t create_vao() {
	auto vao = shared_vao_t(new GLuint{0}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting VAO {}", *ptr);
#endif
		glDeleteVertexArrays(1, ptr);
		delete ptr;
	});
	glGenVertexArrays(1, vao.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating VAO {}", *vao);
#endif
	return vao;
}

[[maybe_unused]] shared_ebo_t create_ebo() {
	auto ebo = shared_ebo_t(new GLuint{0}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting EBO {}", *ptr);
#endif
		glDeleteBuffers(1, ptr);
		delete ptr;
	});
	glGenBuffers(1, ebo.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating EBO {}", *ebo);
#endif
	return ebo;
}

[[maybe_unused]] shared_tex_t create_tex() {
	auto tex = shared_tex_t(new GLuint{0}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting TEX {}", *ptr);
#endif
		glDeleteTextures(1, ptr);
		delete ptr;
	});
	glGenTextures(1, tex.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating TEX {}", *tex);
#endif
	return tex;
}

shared_prog_t create_prog() {
	auto prog = shared_prog_t(new GLuint{glCreateProgram()}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting PROG {}", *ptr);
#endif
		glDeleteProgram(*ptr);
		delete ptr;
	});
#ifdef MEM_DEBUG
	spdlog::trace("Generating PROG {}", *prog);
#endif
	return prog;
}

} // namespace sd3d::memory
