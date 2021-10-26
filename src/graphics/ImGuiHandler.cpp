//
// Created by Tobias on 10/13/2020.
//

#include "ImGuiHandler.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../external/imgui-filebrowser/imfilebrowser.hpp"

namespace sd3d::gui {

void setup_imgui(GLFWwindow *window, bool useIni) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	if (!useIni) {
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

void apply_style() {
	auto *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.60f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.08f, 0.08f, 0.78f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.08f, 0.08f, 0.78f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.14f, 0.14f, 0.78f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.39f, 0.20f, 0.20f, 0.73f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.82f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.68f, 0.15f, 0.15f, 0.84f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.82f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_Button] = ImVec4(0.82f, 0.16f, 0.16f, 0.39f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.66f, 0.21f, 0.21f, 0.84f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.25f, 0.25f, 0.22f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.87f, 0.23f, 0.23f, 0.77f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.43f, 0.43f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.15f, 0.15f, 0.84f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.82f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.82f, 0.16f, 0.16f, 0.31f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.82f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.82f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.50f, 0.18f, 0.18f, 0.84f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.82f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_TabActive] = ImVec4(0.82f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.16f, 0.07f, 0.07f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.38f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.88f, 0.24f, 0.24f, 0.37f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.92f, 0.24f, 0.24f, 1.00f);

	auto &style = ImGui::GetStyle();
	style.WindowPadding = {3, 3};
	style.FramePadding = {4, 1};
	style.ItemSpacing = {9, 4};
	style.ItemInnerSpacing = {4, 4};
	style.TouchExtraPadding = {0, 0};
	style.IndentSpacing = 21.f;
	style.ScrollbarSize = 12.f;
	style.GrabMinSize = 6.f;

	style.WindowBorderSize = 0.f;
	style.ChildBorderSize = 0.f;
	style.PopupBorderSize = 0.f;
	style.FrameBorderSize = 0.f;
	style.TabBorderSize = 0.f;

	style.WindowRounding = 0.f;
	style.ChildRounding = 0.f;
	style.FrameRounding = 0.f;
	style.PopupRounding = 0.f;
	style.ScrollbarRounding = 0.f;
	style.GrabRounding = 0.f;
	style.LogSliderDeadzone = 4.f;
	style.TabRounding = 0.f;
}

} // namespace sd3d::gui
