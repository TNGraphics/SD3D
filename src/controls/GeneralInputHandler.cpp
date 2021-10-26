//
// Created by Tobias on 10/12/2020.
//

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>

#include "GeneralInputHandler.hpp"

// TODO move all the ImGui_ImplGlfw callbacks to the abstract base classes
//  static functions

void GeneralInputHandler::process_mouse_scroll_input(GLFWwindow *win, double x,
													 double y) {
	ImGui_ImplGlfw_ScrollCallback(win, x, y);
	m_dScroll = y;
}

void GeneralInputHandler::process_mouse_press_input(GLFWwindow *win, int button,
													int action,
													[[maybe_unused]] int mods) {
	ImGui_ImplGlfw_MouseButtonCallback(win, button, action, mods);
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			m_mousePressed = true;
		} else if (action == GLFW_RELEASE) {
			m_mousePressed = false;
		}
	}
}

void GeneralInputHandler::process_mouse_pos_input(GLFWwindow *, double x,
												  double y) {
	m_x = x;
	m_y = y;
}

void GeneralInputHandler::process_keyboard_input(GLFWwindow *win, int key,
												 int sc, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(win, key, sc, action, mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(win, true);
	}
	// no other keys in this application :)
}

void GeneralInputHandler::update() {
	// poll events here, because we are handling updates
	glfwPollEvents();

	if (!m_firstUpdate) {
		m_dX = m_x - m_lastX;
		m_dY = m_y - m_lastY;
	} else {
		m_firstUpdate = false;
	}
	m_lastX = m_x;
	m_lastY = m_y;

	m_lastDScroll = m_dScroll;
	m_dScroll = 0.0;
}
