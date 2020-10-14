//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_IMGUIHANDLER_H
#define SD3D_IMGUIHANDLER_H

struct GLFWwindow;
struct ImGuiIO;

namespace sd3d::gui {

void setup_imgui(GLFWwindow *);

void new_frame();

void render();

void shutdown();

ImGuiIO &get_io();

}

#endif //SD3D_IMGUIHANDLER_H
