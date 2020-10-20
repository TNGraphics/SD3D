//
// Created by Tobias on 10/17/2020.
//

#ifndef SD3D_GLCONTEXT_H
#define SD3D_GLCONTEXT_H

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include <imgui.h>

struct GLFWwindow;

namespace sd3d {

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
	};

private:
	static void setup_glfw();
	[[nodiscard("Returns success")]] static bool setup_glad();
	static void setup_debug_output();

	static void framebuffer_resize_cb(GLFWwindow *, int width, int height);

	static int bool_to_gl(bool);

public:
	GlContext() = default; // initialize with init method later
	GlContext(const GlContext &) = delete;
	GlContext(GlContext &&) = delete;
	explicit GlContext(const Settings &settings);
	~GlContext();

	explicit operator bool() const;

	void init(const Settings &settings);

	[[nodiscard]] bool is_open() const;
	void swap_buffer() const;

	// FIXME this setting applies to the current context (which is not
	// necessarily this one)
	static void set_vsync(Settings::Vsync vsync);

	[[nodiscard]] GLFWwindow *win() const { return m_window; }

	[[nodiscard]] int width() const { return m_width; }
	[[nodiscard]] int height() const { return m_height; }
	[[nodiscard]] float aspect() const;

	[[nodiscard]] bool is_current_context() const;

	void clear() const {
		clear(0, 0, 0);
	}
	void clear(const glm::vec3 &col) const {
		clear(col.x, col.y, col.z);
	}
	void clear(const glm::vec4 &col) const {
		clear(col.x, col.y, col.z, col.w);
	}
	void clear(const ImVec4 &col) const {
		clear(col.x, col.y, col.z, col.w);
	}
	void clear(float r, float g, float b, float a = 1.0f) const;

	// TODO more functions for resizing, minimizing, etc.
	// TODO somehow handle resize callback
private:
	bool m_initialized{false};

	GLFWwindow *m_window{};

	int m_width{};
	int m_height{};
};

} // namespace sd3d

#endif // SD3D_GLCONTEXT_H
