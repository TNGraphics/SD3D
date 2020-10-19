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

#include <lyra/lyra.hpp>

#pragma warning(pop)

#include <stb_image.h>

#include <spdlog/spdlog.h>

#include <imgui.h>

#include <gsl-lite/gsl-lite.hpp>

#include "external/imgui-filebrowser/imfilebrowser.h"
#include "graphics/ImGuiHandler.h"

#include "graphics/GlContext.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/data/GlMesh.h"
#include "graphics/data/Model.h"

#include "models/primitives.h"

#include "controls/GeneralInputHandler.h"
#include "controls/OrbitCameraController.h"


float g_mixVal{};
void debug_gui(ImGui::FileBrowser &);

int main(int argc, const char *argv[]) {
	using namespace sd3d;
	spdlog::set_level(spdlog::level::debug);

	std::string resPath{};
	bool showHelp = false;
	int width{1200};
	int height{800};
	bool noVsync{};
	auto cli =
		lyra::opt(resPath, "res-path")["-r"]["--res"]["--path"]["-p"](
			"The path of the res folder relative to the executable") |
		lyra::opt(width, "width")["-w"]["--width"]("The window width") |
		lyra::opt(height, "height")["-h"]["--height"]("The window height") |
		lyra::opt(noVsync)["--disable-vsync"]("Force VSYNC to be disabled") |
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

	GlContext glContext{GlContext::Settings{.width = width, .height = height}};
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

	Shader shader{resPath + "shaders/builtin/generic.vert",
				  resPath + "shaders/builtin/generic.frag"};

	Texture container{resPath + "img/container.jpg"};

	Model monkey{};

	OrbitCameraController cam{
		{70.0, glContext.aspect(), glm::vec3{0, 0, -10}},
		{glm::vec3{}, 10.0f, 2.5f, 20.0f, 1.f, 0.25f, 3.f}};

	shader.use();
	shader.set("texture1", 0);

	double deltaTime;
	double lastFrame{glfwGetTime()};

	fileBrowser.Open();

	while (glContext.is_open()) {
		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		container.bind();

		shader.use();
		shader.set("mixVal", g_mixVal);
		shader.set("view", cam.cam().view());
		shader.set("projection", cam.cam().projection());

		glm::mat4 model{1.0};
		shader.set("model", model);

		monkey.draw();

		gui::new_frame();

		// Do IMGUI stuff here
		debug_gui(fileBrowser);

		if(fileBrowser.HasSelected()) {
			spdlog::info("Opening model: {}", fileBrowser.GetSelected().string());
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
	ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	if(ImGui::Button("Open file browser")) {
		fileBrowser.Open();
	}
	ImGui::End();
}
