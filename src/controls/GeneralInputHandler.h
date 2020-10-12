//
// Created by Tobias on 10/12/2020.
//

#ifndef SD3D_GENERALINPUTHANDLER_H
#define SD3D_GENERALINPUTHANDLER_H

#include "InputHandler.h"

class GeneralInputHandler : public InputHandler {
private:
	bool m_mousePressed{};

	bool m_firstUpdate{true};

	double m_x{};
	double m_y{};

	double m_lastX{};
	double m_lastY{};

	double m_dX{};
	double m_dY{};

	double m_dScroll{};
	double m_lastDScroll{};

	void process_mouse_scroll_input(GLFWwindow *, double, double) override;

	void process_mouse_press_input(GLFWwindow *, int, int, int) override;

	void process_mouse_pos_input(GLFWwindow *, double, double) override;

	void process_keyboard_input(GLFWwindow *, int, int, int, int) override;

public:
	explicit GeneralInputHandler(GLFWwindow *win) : InputHandler{win}, m_firstUpdate{true} {}

	void update() override;

	[[nodiscard]] bool is_mouse_pressed() const { return m_mousePressed; }

	[[nodiscard]] double d_x() const { return m_dX; }

	[[nodiscard]] double d_y() const { return m_dY; }

	[[nodiscard]] double d_scroll() const { return m_lastDScroll; }

};


#endif //SD3D_GENERALINPUTHANDLER_H
