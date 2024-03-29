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
#include "graphics/data/AsyncModel.h"

#include "controls/GeneralInputHandler.h"
#include "controls/OrbitCameraController.h"

void fps_window();

void point_light_gui(unsigned int index, const LitShader &litShader,
					 light::PointLight &lightData);
void dir_light_gui(const LitShader &litShader, light::DirLight &lightData);

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
			"The path to the resource folder, relative to the .exe. Leave "
			"empty if next to it.") |
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

	gl::GlContext glContext{gl::GlContext::Settings{
		.width = width,
		.height = height,
		.vsync = noVsync ? gl::GlContext::Settings::Vsync::NONE
						 : gl::GlContext::Settings::Vsync::SINGLE_BUFFERED,
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

	gui::load_font(resourcePath + "res/fonts/roboto.ttf", 15.0f);
	gui::apply_style();

	LitShader litShader;
	ColorShader lightShader;

	AsyncModel monkey{};
	Model light{resourcePath + "res/light.fbx"};

	OrbitCameraController cam{
		{70.0, glContext.aspect(), glm::vec3{0, 0, -10}},
		{glm::vec3{}, 10.0f, 2.5f, 20.0f, 1.f, 0.25f, 3.f}};

	glContext.set_framebuffer_cb([&c = cam.cam()](float aspect) mutable {
		c.set_aspect(aspect);
	});

	double deltaTime;
	double lastFrame{glfwGetTime()};

	float modelScaleCoarse{1.f};
	float modelScaleFine{1.f};

	glm::vec3 clearCol{0, 0, 0};
	glm::vec3 modelTint{1.0f};

	int shininessExp = 5;
	litShader.bind();
	litShader.set("material.shininess", static_cast<float>(pow(2.f, shininessExp)));

	light::DirLight dirLightSettings{{-0.2f, -1.0f, -0.3f},
									 {1.0f, 0.9f, 1.0f},
									 0.1f,
									 0.4f,
									 0.5f};
	std::array<light::PointLight, 4> pointLightSettings{
		{{{0.f, 3.0f, 3.f}, {1, 0, 0}, 0.8f, 1.f},
		 {{3.f, -3.0f, 0}, {0, 1, 0}, 0.8f, 1.f},
		 {{0.f, 3.0f, -3.f}, {0, 0, 1}, 0.8f, 1.f},
		 {{-3.f, -3.0f, 0}, {1, 0, 1}, 0.8f, 1.f}}};

	for (unsigned int i = 0; i < LitShader::s_plightCount; ++i) {
		litShader.set_plight_attenuation(i, light::from_distance(50));
	}

	litShader.set_dir_light(dirLightSettings);
	litShader.set_plights(pointLightSettings);

	bool drawLights = true;
	bool showFps = false;
	bool showLightSettings = false;
	bool showModelSettings = false;
	bool showCameraSettings = false;

	while (glContext.is_open()) {
		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glContext.clear(clearCol);

		litShader.bind();

		litShader.view(cam.ccam().view());
		litShader.projection(cam.ccam().projection());
		litShader.set_cam_pos(cam.ccam().get_pos());

		litShader.set_color(modelTint);

		monkey.draw(litShader);

		if (drawLights) {
			lightShader.bind();
			lightShader.view(cam.ccam().view());
			lightShader.projection(cam.ccam().projection());

			glm::mat4 model{1.0};

			for (const auto &l : pointLightSettings) {
				model = glm::mat4{1.0f};
				model = glm::translate(model, l.position);
				model = glm::scale(model, glm::vec3(0.35f));
				lightShader.model(model);

				lightShader.set_color(l.color);

				light.draw();
			}
		}

		gui::new_frame();

		if(showModelSettings) {
			ImGui::SetNextWindowSize({-1, -1});
			ImGui::Begin("Misc Settings", &showModelSettings,
						 ImGuiWindowFlags_NoResize);
			if(ImGui::InputFloat("Scale", &modelScaleCoarse, 0.001f, 0.05f)) {
				auto model{glm::scale(glm::mat4{1.0}, glm::vec3{modelScaleCoarse * modelScaleFine})};
				monkey.apply_transform(model);
			}
			if(ImGui::SliderFloat("Scale - Fine", &modelScaleFine, 0.25f, 5.f)) {
				auto model{glm::scale(glm::mat4{1.0}, glm::vec3{modelScaleCoarse * modelScaleFine})};
				monkey.apply_transform(model);
			}
			if (ImGui::SliderInt("Shininess", &shininessExp, 0, 8)) {
				litShader.bind();
				litShader.set("material.shininess", static_cast<float>(pow(2.f, shininessExp)));
			}
			ImGui::ColorEdit3("Model Tint", glm::value_ptr(modelTint));
			ImGui::End();
		}

		if(showCameraSettings) {
			cam.settings_gui(showCameraSettings, clearCol);
		}

		if(showLightSettings) {
			ImGui::SetNextWindowSizeConstraints({300, -1}, {FLT_MAX, -1});
			if (ImGui::Begin("Light Settings", &showLightSettings,
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
				if (ImGui::CollapsingHeader("Directional Light")) {
					dir_light_gui(litShader, dirLightSettings);
				}
				if (ImGui::CollapsingHeader("Point Lights")) {
					ImGui::Checkbox("Draw Lights", &drawLights);
					if (ImGui::TreeNode("Light 1")) {
						point_light_gui(0, litShader, pointLightSettings.at(0));
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Light 2")) {
						point_light_gui(1, litShader, pointLightSettings.at(1));
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Light 3")) {
						point_light_gui(2, litShader, pointLightSettings.at(2));
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Light 4")) {
						point_light_gui(3, litShader, pointLightSettings.at(3));
						ImGui::TreePop();
					}
				}
			}
			ImGui::End();
		}

		if(showFps) {
			fps_window();
		}

		ImGui::BeginMainMenuBar();
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("Open...")) {
				// TODO also add shortcut
				fileBrowser.Open();
			}
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Settings")) {
			ImGui::MenuItem("Show Light Settings", nullptr, &showLightSettings);
			ImGui::MenuItem("Show Model Settings", nullptr, &showModelSettings);
			ImGui::MenuItem("Show Camera Settings", nullptr, &showCameraSettings);
			ImGui::Separator();
			ImGui::MenuItem("Show FPS", nullptr, &showFps);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		if (fileBrowser.HasSelected()) {
			spdlog::info("Opening model: {}",
						 fileBrowser.GetSelected().string());
			monkey = AsyncModel{fileBrowser.GetSelected(), glm::scale(glm::mat4{1.0}, glm::vec3{modelScaleCoarse})};
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

void point_light_gui(unsigned int index, const LitShader &litShader,
					 light::PointLight &lightData) {
	// TODO make it possible to disable light
	litShader.bind();
	if (ImGui::DragFloat3("Position", glm::value_ptr(lightData.position),
						  0.1f)) {
		litShader.set_plight_position(index, lightData.position);
	}
	if (ImGui::ColorEdit3("Color", glm::value_ptr(lightData.color))) {
		litShader.set_plight_color(index, lightData.color);
	}
	if (ImGui::SliderFloat("Diffuse", &lightData.diffuse, 0.f, 1.f)) {
		litShader.set_plight_diffuse(index, lightData.diffuse);
	}
	if (ImGui::SliderFloat("Specular", &lightData.specular, 0.f, 1.f)) {
		litShader.set_plight_specular(index, lightData.specular);
	}
	Shader::unbind();
}

void dir_light_gui(const LitShader &litShader, light::DirLight &lightData) {
	// TODO make it possible to disable light
	litShader.bind();
	if (ImGui::DragFloat3("Direction", glm::value_ptr(lightData.direction),
						  0.1f)) {
		litShader.set_dir_light_direction(glm::normalize(lightData.direction));
	}
	if (ImGui::ColorEdit3("Color", glm::value_ptr(lightData.color))) {
		litShader.set_dir_light_color(lightData.color);
	}
	if (ImGui::SliderFloat("Ambient", &lightData.ambient, 0.f, 1.f)) {
		litShader.set_dir_light_ambient(lightData.ambient);
	}
	if (ImGui::SliderFloat("Diffuse", &lightData.diffuse, 0.f, 1.f)) {
		litShader.set_dir_light_diffuse(lightData.diffuse);
	}
	if (ImGui::SliderFloat("Specular", &lightData.specular, 0.f, 1.f)) {
		litShader.set_dir_light_specular(lightData.specular);
	}
	Shader::unbind();
}

void fps_window() {
	static constexpr float dist = 10.0f;
	static ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	auto &io = ImGui::GetIO();
	ImVec2 windowPos{io.DisplaySize.x - dist, io.DisplaySize.y - dist};
	ImVec2 windowPivot{1.f, 1.f};
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::Begin("FPS", nullptr, windowFlags);
	ImGui::Value("FPS", io.Framerate);
	ImGui::End();
}
