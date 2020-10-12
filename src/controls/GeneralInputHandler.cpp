//
// Created by Tobias on 10/12/2020.
//

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include "GeneralInputHandler.h"

void GeneralInputHandler::process_mouse_scroll_input(GLFWwindow *, [[maybe_unused]] double x, double y) {
	m_dScroll = y;
}

void GeneralInputHandler::process_mouse_press_input(GLFWwindow *, int button, int action, [[maybe_unused]] int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			m_mousePressed = true;
		} else if (action == GLFW_RELEASE) {
			m_mousePressed = false;
		}
	}
}

void GeneralInputHandler::process_mouse_pos_input(GLFWwindow *, double x, double y) {
	m_x = x;
	m_y = y;
}

void GeneralInputHandler::process_keyboard_input(GLFWwindow *win, int key, int, int action, int) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwWindowShouldClose(win);
	}
	// no other keys in this application :)
}

void GeneralInputHandler::update() {
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
