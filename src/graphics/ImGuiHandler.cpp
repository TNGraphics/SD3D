//
// Created by Tobias on 10/13/2020.
//

#include "ImGuiHandler.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../external/imgui-filebrowser/imfilebrowser.h"

namespace sd3d::gui {

void setup_imgui(GLFWwindow *window, bool useIni) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	if(!useIni) {
		ImGui::GetIO().IniFilename = nullptr;
	}

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

void new_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImGuiIO &get_io() {
	return ImGui::GetIO();
}

void shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

ImFont *load_font(const std::string &path, float size) {
	auto ptr{get_io().Fonts->AddFontFromFileTTF(path.c_str(), size)};
	get_io().Fonts->Build();
	return ptr;
}

void push_font(ImFont *font) {
	ImGui::PushFont(font);
}

void pop_font() {
	ImGui::PopFont();
}

} // namespace sd3d::gui
