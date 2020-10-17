//
// Created by Tobias on 10/17/2020.
//
// ORDER IS IMPORTANT!!
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <numeric>

#include <spdlog/spdlog.h>

#include "GlContext.h"

namespace sd3d {

GlContext::GlContext(const GlContext::Settings &s) {
	init(s);
}

GlContext::~GlContext() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void GlContext::init(const GlContext::Settings &s) {
	if (m_initialized) return;

	setup_glfw();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//	glfwWindowHint(GLFW_RESIZABLE, bool_to_gl(s.resizable));
	glfwWindowHint(GLFW_DECORATED, bool_to_gl(s.decorated));

	m_width = s.width;
	m_height = s.height;

	m_window = glfwCreateWindow(m_width, m_height, s.title, nullptr, nullptr);
	if (!m_window) {
		spdlog::error("Window could not be created!");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_window);

	glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_cb);

	if (!setup_glad()) {
		return;
	}

	setup_debug_output();

	glViewport(0, 0, m_width, m_height);
	// TODO handle frame buffer resize

	set_vsync(s.vsync);

	glEnable(GL_DEPTH_TEST);

	// at the end (when creation was successful) set m_initialized to true
	m_initialized = true;
}

void GlContext::setup_glfw() {
	// only needs to happen once
	static bool isSetUp = false;
	if (isSetUp) return;

	// add an error callback
	glfwSetErrorCallback([](auto error, const auto *desc) {
		spdlog::error("GLFW ERROR [{}]: {}", error, desc);
	});

	glfwInit();

	// initialize OpenGL in the correct version (OpenGL 4.6 Core profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	isSetUp = true;
}

bool GlContext::setup_glad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		spdlog::error("Error loading GLAD!");
		glfwTerminate();
		return false;
	} else {
		return true;
	}
}

void GlContext::set_vsync(GlContext::Settings::Vsync vsync) {
	switch (vsync) {
	case Settings::Vsync::NONE:
		glfwSwapInterval(0);
		break;
	case Settings::Vsync::SINGLE_BUFFERED:
		glfwSwapInterval(1);
		break;
	case Settings::Vsync::DOUBLE_BUFFERED:
		glfwSwapInterval(2);
		break;
	}
}

void GlContext::swap_buffer() const {
	glfwSwapBuffers(win());
}

float GlContext::aspect() const {
	if (height() == 0) {
		return std::numeric_limits<float>::infinity();
	} else {
		return static_cast<float>(width()) / static_cast<float>(height());
	}
}

GlContext::operator bool() const {
	// TODO return if object is valid
	return m_initialized;
}

int GlContext::bool_to_gl(bool b) {
	if (b) {
		return GL_TRUE;
	} else {
		return GL_FALSE;
	}
}

void GlContext::framebuffer_resize_cb(GLFWwindow *, int width, int height) {
	// FIXME needs to update current GlContext -> not static
	// Use same system as InputHandler -> GetUserAttribPointer
	glViewport(0, 0, width, height);
}

bool GlContext::is_open() const {
	return !glfwWindowShouldClose(m_window);
}

void GLAPIENTRY opengl_message_callback([[maybe_unused]] GLenum source,
										GLenum type, [[maybe_unused]] GLuint id,
										GLenum severity,
										[[maybe_unused]] GLsizei length,
										const GLchar *message,
										[[maybe_unused]] const void *userParam);

void GlContext::setup_debug_output() {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_message_callback, nullptr);
}

void GLAPIENTRY opengl_message_callback(
	[[maybe_unused]] GLenum source, GLenum type, [[maybe_unused]] GLuint id,
	GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message,
	[[maybe_unused]] const void *userParam) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		spdlog::error("GL ERROR: type {} message: {}", type, message);
#if DEBUG
	} else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
		spdlog::warn("GL WARNING: type {} message: {}", type, message);
	}
#else
	}
#endif
}

} // namespace sd3d
