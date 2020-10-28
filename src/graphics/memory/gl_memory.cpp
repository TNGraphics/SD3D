//
// Created by Tobias on 10/20/2020.
//
#include <glad/glad.h>

#ifdef MEM_DEBUG
#include <spdlog/spdlog.h>
#endif

#include "gl_memory.h"

namespace sd3d::memory::detail {

buffer_ptr_t VboCreator::operator()() {
	auto vbo = buffer_ptr_t(new GLuint{0}, [](const GLuint *ptr) {
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

buffer_ptr_t VaoCreator::operator()() {
	auto vao = buffer_ptr_t(new GLuint{0}, [](const GLuint *ptr) {
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

buffer_ptr_t EboCreator::operator()() {
	auto ebo = buffer_ptr_t(new GLuint{0}, [](const GLuint *ptr) {
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

buffer_ptr_t TexCreator::operator()() {
	auto tex = buffer_ptr_t(new GLuint{0}, [](const GLuint *ptr) {
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

buffer_ptr_t ProgCreator::operator()() {
	auto prog = buffer_ptr_t(new GLuint{glCreateProgram()}, [](const GLuint *ptr) {
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

buffer_ptr_t FboCreator::operator()() {
	auto fbo = buffer_ptr_t(new GLuint{glCreateProgram()}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting FBO {}", *ptr);
#endif
		glDeleteFramebuffers(1, ptr);
		delete ptr;
	});
	glGenFramebuffers(1, fbo.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating FBO {}", *fbo);
#endif
	return fbo;
}

buffer_ptr_t RboCreator::operator()() {
	auto rbo = buffer_ptr_t(new GLuint{glCreateProgram()}, [](const GLuint *ptr) {
#ifdef MEM_DEBUG
		spdlog::trace("Deleting RBO {}", *ptr);
#endif
		glDeleteRenderbuffers(1, ptr);
		delete ptr;
	});
	glGenRenderbuffers(1, rbo.get());
#ifdef MEM_DEBUG
	spdlog::trace("Generating RBO {}", *rbo);
#endif
	return rbo;
}

} // namespace sd3d::memory
