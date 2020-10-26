//
// Created by Tobias on 10/12/2020.
//

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "../graphics/GlContext.h"

#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow *win) {
	// TODO maybe create a containing object so that more than one thing
	auto *winData = sd3d::gl::get_or_create_window_data(win);
	if (!winData->inputHandler) {
		winData->inputHandler = this;
		glfwSetKeyCallback(win, s_process_keyboard_input);
		glfwSetMouseButtonCallback(win, s_process_mouse_press_input);
		glfwSetCursorPosCallback(win, s_process_mouse_pos_input);
		glfwSetScrollCallback(win, s_process_mouse_scroll_input);
	} else {
		spdlog::warn("Trying to add InputHandler to window that already has "
					 "one, ignoring the new one!");
	}
}

void InputHandler::s_process_keyboard_input(GLFWwindow *win, int key,
											int scancode, int action,
											int mods) {
	auto handler = sd3d::gl::get_or_create_window_data(win)->inputHandler;
	if (handler) {
		handler->process_keyboard_input(win, key, scancode, action, mods);
	}
}

void InputHandler::s_process_mouse_press_input(GLFWwindow *win, int button,
											   int action, int mods) {
	auto handler = sd3d::gl::get_or_create_window_data(win)->inputHandler;
	if (handler) {
		handler->process_mouse_press_input(win, button, action, mods);
	}
}

void InputHandler::s_process_mouse_pos_input(GLFWwindow *win, double x,
											 double y) {
	auto handler = sd3d::gl::get_or_create_window_data(win)->inputHandler;
	if (handler) {
		handler->process_mouse_pos_input(win, x, y);
	}
}

void InputHandler::s_process_mouse_scroll_input(GLFWwindow *win, double x,
												double y) {
	auto handler = sd3d::gl::get_or_create_window_data(win)->inputHandler;
	if (handler) {
		handler->process_mouse_scroll_input(win, x, y);
	}
}
