//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_IMGUIHANDLER_H
#define SD3D_IMGUIHANDLER_H

#include <imgui.h>
#include <string>

struct GLFWwindow;

namespace sd3d::gui {

void setup_imgui(GLFWwindow *, bool useIni = true);

void new_frame();

void render();

void shutdown();

ImFont *load_font(const std::string &path, float size = 11.0f);

void push_font(ImFont *);

void pop_font();

ImGuiIO &get_io();

void apply_style();

} // namespace sd3d::gui

#endif // SD3D_IMGUIHANDLER_H
