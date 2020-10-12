//
// Created by Tobias on 10/12/2020.
//

#ifndef SD3D_GENERALINPUTHANDLER_H
#define SD3D_GENERALINPUTHANDLER_H

#include "InputHandler.h"

class GeneralInputHandler : public InputHandler {
public:
	explicit GeneralInputHandler(GLFWwindow *win) : InputHandler{win} {}

	void process_mouse_scroll_input(GLFWwindow *, double, double) override {}

	void process_mouse_press_input(GLFWwindow *, int, int, int) override {}

	void process_mouse_pos_input(GLFWwindow *, double, double) override {}

	void process_keyboard_input(GLFWwindow *, int, int, int, int) override {}
};


#endif //SD3D_GENERALINPUTHANDLER_H
