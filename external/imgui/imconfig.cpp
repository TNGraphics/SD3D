//
// Created by Tobias on 10/18/2021.
//

#include "imconfig.h"

#define IMGUI_DEFINE_MATH_OPERATORS = 1

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {
void PaddedText(const char *text, float paddingX, float paddingY) {
	const auto padding = ImVec2{paddingX, paddingY};
	ImVec2 sz = ImGui::CalcTextSize(text);
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::InvisibleButton("##padded-text", sz + (padding * 2));    // ImVec2 operators require imgui_internal.h include and -DIMGUI_DEFINE_MATH_OPERATORS=1
	ImVec2 finalCursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(cursor + padding);
	ImGui::Text("%s", text);
	ImGui::SetCursorPos(finalCursorPos);
}

void PaddedText(const char *text, float top, float bottom, float left, float right) {
	const auto paddingWhole = ImVec2{left + right, top + bottom};
	ImVec2 sz = ImGui::CalcTextSize(text);
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::InvisibleButton("##padded-text", sz + paddingWhole);    // ImVec2 operators require imgui_internal.h include and -DIMGUI_DEFINE_MATH_OPERATORS=1
	ImVec2 finalCursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(cursor + ImVec2{left, top});
	ImGui::Text("%s", text);
	ImGui::SetCursorPos(finalCursorPos);
}
}
