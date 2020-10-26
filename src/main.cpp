//
// Created by Tobias on 9/20/2020.
//
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <lyra/lyra.hpp>

#pragma warning(pop)

#include <stb_image.h>

#include <spdlog/spdlog.h>

#include <imgui.h>

#include <gsl-lite/gsl-lite.hpp>

#include "external/imgui-filebrowser/imfilebrowser.h"
#include "graphics/ImGuiHandler.h"

#include "graphics/GlContext.h"
#include "graphics/data/Texture.h"

#include "graphics/shaders/ColorShader.h"
#include "graphics/shaders/LitShader.h"

#include "graphics/data/GlMesh.h"
#include "graphics/data/Model.h"

#include "controls/GeneralInputHandler.h"
#include "controls/OrbitCameraController.h"

void debug_gui(ImGui::FileBrowser &);

int main(int argc, const char *argv[]) {
	using namespace sd3d;

#ifdef MEM_DEBUG
	spdlog::set_level(spdlog::level::trace);
#elif DEBUG
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::err);
#endif
	bool showHelp = false;
	int width{1200};
	int height{800};
	bool noVsync{};
	std::string resourcePath{};
	auto cli =
		lyra::opt(width, "width")["-w"]["--width"]("The window width") |
		lyra::opt(height, "height")["-h"]["--height"]("The window height") |
		lyra::opt(noVsync)["--disable-vsync"]("Force VSYNC to be disabled") |
		lyra::opt(resourcePath, "resource path")["-r"]["--res"](
			"The path to the resource folder, relative to the .exe. Leave empty if next to it.") |
		lyra::help(showHelp);

	auto parsed = cli.parse({argc, argv});

	if (!parsed) {
		spdlog::error("CLI argument parsing error: {}", parsed.errorMessage());
		return 1;
	}

	if (showHelp) {
		std::cout << cli << '\n';
		return 0;
	}

	stbi_set_flip_vertically_on_load(true);

	GlContext glContext{GlContext::Settings{
		.width = width,
		.height = height,
		.vsync = noVsync ? GlContext::Settings::Vsync::NONE
						 : GlContext::Settings::Vsync::SINGLE_BUFFERED,
		.title = "SD3D"}};
	if (!glContext) {
		spdlog::error("Error setting up GL Context!");
		return -1;
	}

	gui::setup_imgui(glContext.win());

	ImGui::FileBrowser fileBrowser;
	fileBrowser.SetTitle("Select a model");
	fileBrowser.SetTypeFilters({".obj", ".fbx", ".dae", ".gltf", ".glb", ".3ds",
								".ase", ".ifc", ".xgl", ".zgl", ".ply", ".lwo",
								".lws", ".lxo", ".stl", ".x", ".ac", ".ms3d"});

	GeneralInputHandler inputHandler{glContext.win()};

	gui::load_font(resourcePath + "/res/fonts/roboto.ttf", 15.0f);

	LitShader litShader;
	ColorShader lightShader;

	Model monkey{};
	Model light{Model::from_path(resourcePath + "/res/light.fbx")};
	glm::vec3 lightPos{0.f, 3.0f, 2.0f};
	glm::vec3 lightCol{1, 0.4f, 1};

	OrbitCameraController cam{
		{70.0, glContext.aspect(), glm::vec3{0, 0, -10}},
		{glm::vec3{}, 10.0f, 2.5f, 20.0f, 1.f, 0.25f, 3.f}};

	litShader.bind();

	litShader.set_dir_light_direction(glm::vec3(-0.2f, -1.0f, -0.3f));
	litShader.set_dir_light_ambient(0.1f);
	litShader.set_dir_light_diffuse(0.4f);
	litShader.set_dir_light_specular(0.5f);
	litShader.set_dir_light_color(glm::vec3(1.0f, 0.9f, 1.0f));

	litShader.set_plight_diffuse(0, 0.8f);
	litShader.set_plight_specular(0, 1);
	litShader.set_plight_color(0, glm::vec3(1, 0.4f, 1));
	litShader.set_plight_attenuation(0, light::from_distance(500));

	lightShader.bind();
	lightShader.set("color", lightCol);

	double deltaTime;
	double lastFrame{glfwGetTime()};

	float modelScale{1.0f};

	ImVec4 clearCol{0, 0, 0, 1.0f};
	glm::vec3 modelTint{1.0f};

	int shininessExp = 5;
	litShader.bind();
	litShader.set("material.shininess", pow(2.f, shininessExp));

	while (glContext.is_open()) {
		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glContext.clear(clearCol);

		litShader.bind();

		litShader.view(cam.cam().view());
		litShader.projection(cam.cam().projection());
		litShader.set_cam_pos(cam.cam().get_pos());
		litShader.set_plight_position(0, lightPos);
		litShader.set_plight_color(0, lightCol);

		glm::mat4 model{1.0};
		model = glm::scale(model, glm::vec3(modelScale));
		litShader.model(model);
		glm::mat3 normalMatrix{glm::transpose(glm::inverse(model))};
		litShader.normal_mat(normalMatrix);

		litShader.set_color(modelTint);
		litShader.set("material.shininess", 1.f);

		monkey.draw(litShader);

		lightShader.bind();
		lightShader.view(cam.cam().view());
		lightShader.projection(cam.cam().projection());

		model = glm::mat4{1.0f};
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.35f));
		lightShader.model(model);

		lightShader.set_color(lightCol);

		light.draw();

		gui::new_frame();

		ImGui::SetNextWindowSize({400, -1});
		ImGui::Begin("Model Settings", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::Text("Scale");
		ImGui::SliderFloat("Scale", &modelScale, 0.01f, 100.f);
		if(ImGui::SliderInt("Shininess", &shininessExp, 0, 8)) {
			litShader.bind();
			litShader.set("material.shininess", pow(2.f, shininessExp));
		}
		if(ImGui::CollapsingHeader("Colors")) {
			if(ImGui::TreeNode("Clear Color")) {
				ImGui::ColorPicker3("Clear Color", (float *)&clearCol);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Model Tint")) {
				ImGui::ColorPicker3("Model Tint", glm::value_ptr(modelTint));
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Light color")) {
				ImGui::ColorPicker3("Light Color", glm::value_ptr(lightCol));
				ImGui::TreePop();
			}
		}
		ImGui::End();

		// Do IMGUI stuff here
		debug_gui(fileBrowser);

		if (fileBrowser.HasSelected()) {
			spdlog::info("Opening model: {}",
						 fileBrowser.GetSelected().string());
			monkey = Model::from_path(fileBrowser.GetSelected().string());
			fileBrowser.Close();
		}

		fileBrowser.Display();

		gui::render();

		glContext.swap_buffer();
		inputHandler.update();

		if (!gui::get_io().WantCaptureMouse) {
			if (inputHandler.is_mouse_pressed()) {
				cam.rotate(inputHandler.d_x(), inputHandler.d_y());
			}
			cam.zoom(inputHandler.d_scroll());
		}
		cam.update(gsl::narrow_cast<float>(deltaTime));
	}

	gui::shutdown();

	return 0;
}

void debug_gui(ImGui::FileBrowser &fileBrowser) {
	ImGui::SetNextWindowSize({-1, -1});
	ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	if (ImGui::Button("Open file browser")) {
		fileBrowser.Open();
	}
	ImGui::End();
}
