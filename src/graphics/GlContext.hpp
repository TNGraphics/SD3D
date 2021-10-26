//
// Created by Tobias on 10/17/2020.
//

#ifndef SD3D_GLCONTEXT_H
#define SD3D_GLCONTEXT_H

#include <functional>

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <utility>

#pragma warning(pop)

#include <imgui.h>

struct GLFWwindow;
class InputHandler;

namespace sd3d::gl {

class GlContext;

struct WindowData {
	GlContext *context{nullptr};
	InputHandler *inputHandler{nullptr};
};

WindowData *get_or_create_window_data(GLFWwindow *);

class GlContext {
public:
	struct Settings {
		enum class Vsync { NONE, SINGLE_BUFFERED, DOUBLE_BUFFERED };
		int width{600};
		int height{600};
		Vsync vsync{Vsync::SINGLE_BUFFERED};
		bool resizable{false};
		bool decorated{true};
		const char *title{""};
		int minWidth{480};
		int minHeight{340};
	};

	using framebuffer_resize_cb_t = std::function<void(float)>;

private:
	static void setup_glfw();
	[[nodiscard("Returns success")]] static bool setup_glad();
	static void setup_debug_output();

	framebuffer_resize_cb_t m_framebufferResizeCb{};
	void framebuffer_resize_cb_instance(int width, int height);
	static void framebuffer_resize_cb(GLFWwindow *, int width, int height);

	static int bool_to_gl(bool);

public:
	GlContext() = default; // initialize with init method later
	GlContext(const GlContext &) = delete;
	GlContext(GlContext &&) = delete;
	// FIXME no copy and move assignment
	explicit GlContext(const Settings &settings);
	~GlContext();

	explicit operator bool() const;

	void init(const Settings &settings);

	[[nodiscard]] bool is_open() const;
	void swap_buffer();

	// FIXME this setting applies to the current context (which is not
	//  necessarily this one)
	static void set_vsync(Settings::Vsync vsync);

	[[nodiscard]] GLFWwindow *win() { return m_window; }

	[[nodiscard]] int width() const { return m_width; }
	[[nodiscard]] int height() const { return m_height; }
	[[nodiscard]] float aspect() const;

	[[nodiscard]] bool is_current_context() const;

	// FIXME why is clear const? (changes window)
	void clear() const { clear(0, 0, 0); }
	void clear(const glm::vec3 &col) const { clear(col.x, col.y, col.z); }
	void clear(const glm::vec4 &col) const {
		clear(col.x, col.y, col.z, col.w);
	}
	void clear(const ImVec4 &col) const { clear(col.x, col.y, col.z, col.w); }
	void clear(float r, float g, float b, float a = 1.0f) const;

	void set_framebuffer_cb(framebuffer_resize_cb_t cb) {
		m_framebufferResizeCb = std::move(cb);
	}

	// TODO more functions for resizing, minimizing, etc.
	// TODO somehow handle resize callback
private:
	bool m_initialized{false};

	GLFWwindow *m_window{};

	int m_width{};
	int m_height{};
};

} // namespace sd3d::gl

#endif // SD3D_GLCONTEXT_H
