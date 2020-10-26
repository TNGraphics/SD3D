//
// Created by Tobias on 10/12/2020.
//

#ifndef SD3D_INPUTHANDLER_H
#define SD3D_INPUTHANDLER_H

struct GLFWwindow;

class InputHandler {
public:
	explicit InputHandler(GLFWwindow *);
	virtual ~InputHandler() = default;

	virtual void update() {}

private:
	// static callbacks
	static void s_process_keyboard_input(GLFWwindow *, int, int, int, int);

	static void s_process_mouse_press_input(GLFWwindow *, int, int, int);

	static void s_process_mouse_pos_input(GLFWwindow *, double, double);

	static void s_process_mouse_scroll_input(GLFWwindow *, double, double);

	// callbacks
	virtual void process_keyboard_input(GLFWwindow *, int, int, int, int) = 0;

	virtual void process_mouse_press_input(GLFWwindow *, int, int, int) = 0;

	virtual void process_mouse_pos_input(GLFWwindow *, double, double) = 0;

	virtual void process_mouse_scroll_input(GLFWwindow *, double, double) = 0;
};

#endif // SD3D_INPUTHANDLER_H
