//
// Created by Tobias on 10/12/2020.
//

#ifndef SD3D_INPUTHANDLER_H
#define SD3D_INPUTHANDLER_H

struct GLFWwindow;

// TODO maybe make it virtual/abstract so you can implement functions as you want
class InputHandler {
// This class only handles mouse input and the escape key, no other keys are needed
private:
	bool m_mousePressed{};

	GLFWwindow *m_window;

public:
	explicit InputHandler(GLFWwindow *);
	virtual ~InputHandler() = default;

	void update();

	[[nodiscard]] bool is_mouse_pressed() const { return m_mousePressed; }

// static callbacks
private:
	static void s_process_keyboard_input(GLFWwindow *, int, int, int, int);

	static void s_process_mouse_press_input(GLFWwindow *, int, int, int);

	static void s_process_mouse_pos_input(GLFWwindow *, double, double);

	static void s_process_mouse_scroll_input(GLFWwindow *, double, double);

// callbacks
public:
	virtual void process_keyboard_input(GLFWwindow *, int, int, int, int) = 0;

	virtual void process_mouse_press_input(GLFWwindow *, int, int, int) = 0;

	virtual void process_mouse_pos_input(GLFWwindow *, double, double) = 0;

	virtual void process_mouse_scroll_input(GLFWwindow *, double, double) = 0;
};


#endif //SD3D_INPUTHANDLER_H
