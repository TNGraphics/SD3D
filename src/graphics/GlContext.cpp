//
// Created by Tobias on 10/17/2020.
//
// ORDER IS IMPORTANT!!
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <numeric>

#include <spdlog/spdlog.h>

#include "GlContext.hpp"

namespace sd3d::gl {

WindowData *get_or_create_window_data(GLFWwindow *win) {
	auto *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win));
	if (!data) {
		// create new data
		data = new WindowData{};
		glfwSetWindowUserPointer(win, data);
	}
	return data;
}

GlContext::GlContext(const GlContext::Settings &s) {
	init(s);
}

GlContext::~GlContext() {
	// otherwise we would leak memory
	// maybe we create window data here and immediately delete it, but that
	// shouldn't happen
	// TODO maybe also remove data from window user pointer
	delete get_or_create_window_data(m_window);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void GlContext::init(const GlContext::Settings &s) {
	if (m_initialized) return;

	setup_glfw();
//	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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
	// Window was just created there SHOULDN'T be another window here
	get_or_create_window_data(m_window)->context = this;

	glfwSetWindowSizeLimits(m_window, s.minWidth, s.minHeight, GLFW_DONT_CARE,
							GLFW_DONT_CARE);

	if (!setup_glad()) {
		return;
	}

	setup_debug_output();

	glViewport(0, 0, m_width, m_height);

	set_vsync(s.vsync);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

void GlContext::swap_buffer() {
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

void GlContext::framebuffer_resize_cb(GLFWwindow *win, int width, int height) {
	auto *context = get_or_create_window_data(win)->context;
	if (context) {
		context->framebuffer_resize_cb_instance(width, height);
	}
}

void GlContext::framebuffer_resize_cb_instance(int width, int height) {
	glViewport(0, 0, width, height);
	m_width = width;
	m_height = height;
	if(m_framebufferResizeCb) {
		m_framebufferResizeCb(aspect());
	}
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

void GlContext::clear(float r, float g, float b, float a) const {
	if (!is_current_context()) return;
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool GlContext::is_current_context() const {
	return glfwGetCurrentContext() == m_window;
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

} // namespace sd3d::gl
