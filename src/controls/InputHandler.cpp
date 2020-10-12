//
// Created by Tobias on 10/12/2020.
//

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow *win) : m_window{win} {
	// TODO maybe create a containing object so that more than one thing
	glfwSetWindowUserPointer(win, this);

	glfwSetKeyCallback(win, s_process_keyboard_input);
	glfwSetMouseButtonCallback(win, s_process_mouse_press_input);
	glfwSetCursorPosCallback(win, s_process_mouse_pos_input);
	glfwSetScrollCallback(win, s_process_mouse_scroll_input);
}

void InputHandler::s_process_keyboard_input(GLFWwindow *win, int key, int scancode, int action, int mods) {
	auto handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(win));
	if (handler) {
		handler->process_keyboard_input(win, key, scancode, action, mods);
	}
}

void InputHandler::s_process_mouse_press_input(GLFWwindow *win, int button, int action, int mods) {
	auto handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(win));
	if (handler) {
		handler->process_mouse_press_input(win, button, action, mods);
	}
}

void InputHandler::s_process_mouse_pos_input(GLFWwindow *win, double x, double y) {
	auto handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(win));
	if (handler) {
		handler->process_mouse_pos_input(win, x, y);
	}
}

void InputHandler::s_process_mouse_scroll_input(GLFWwindow *win, double x, double y) {
	auto handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(win));
	if (handler) {
		handler->process_mouse_scroll_input(win, x, y);
	}
}
