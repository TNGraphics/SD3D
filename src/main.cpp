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
#include "graphics/data/Shader.h"
#include "graphics/data/Texture.h"

#include "graphics/data/GlMesh.h"
#include "graphics/data/Model.h"

#include "models/primitives.h"

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
	std::string shaderPath{};
	std::string texturePath{};
	auto cli =
		lyra::opt(width, "width")["-w"]["--width"]("The window width") |
		lyra::opt(height, "height")["-h"]["--height"]("The window height") |
		lyra::opt(noVsync)["--disable-vsync"]("Force VSYNC to be disabled") |
		lyra::opt(shaderPath, "shader path")["-s"]["--shader"](
			"The shader path for developing shaders you might use") |
		lyra::opt(texturePath,
				  "texture path")["-t"]["--texture"]("The texture path") |
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

	gui::setup_imgui(glContext.win(), false);
	ImGui::FileBrowser fileBrowser;
	fileBrowser.SetTitle("Select a model");
	fileBrowser.SetTypeFilters({".obj", ".fbx", ".dae", ".gltf", ".glb", ".3ds",
								".ase", ".ifc", ".xgl", ".zgl", ".ply", ".lwo",
								".lws", ".lxo", ".stl", ".x", ".ac", ".ms3d"});

	GeneralInputHandler inputHandler{glContext.win()};

	Texture diffuseTex{texturePath + "container_diffuse.png", GL_TEXTURE0};
	Texture specularTex{texturePath + "container_specular.png", GL_TEXTURE1};

	Shader litShader{shaderPath + "lit.vert", shaderPath + "lit.frag"};

	Shader lightShader{shaderPath + "light.vert", shaderPath + "light.frag"};

	Model monkey{};

	GlMesh lightMesh{GlMesh::from_data(models::cube_layout(), models::g_cube,
									   sizeof(models::g_cube))};
	glm::vec3 lightPos{0.f, 3.0f, 2.0f};

	OrbitCameraController cam{
		{70.0, glContext.aspect(), glm::vec3{0, 0, -10}},
		{glm::vec3{}, 10.0f, 2.5f, 20.0f, 1.f, 0.25f, 3.f}};

	litShader.bind();
	litShader.set("material.shininess", 32.f);
	litShader.set("material.diffuse", 0);
	litShader.set("material.specular", 1);

	litShader.set("dirLight.direction", -0.2f, -1.0f, -0.3f);
	litShader.set("dirLight.ambient", 0.1f);
	litShader.set("dirLight.diffuse", 0.4f);
	litShader.set("dirLight.specular", 0.5f);
	litShader.set("dirLight.color", 1.0f, 0.9f, 1.0f);

	litShader.set("pointLights[0].diffuse", 0.8f);
	litShader.set("pointLights[0].specular", 1.0f);
	litShader.set("pointLights[0].color", glm::vec3(1.0));
	litShader.set("pointLights[0].constant", 1.0f);
	litShader.set("pointLights[0].linear", 0.09f);
	litShader.set("pointLights[0].quadratic", 0.032f);

	lightShader.bind();
	lightShader.set("color", glm::vec3(1.0f));

	double deltaTime;
	double lastFrame{glfwGetTime()};

	fileBrowser.Open();

	float modelScale{1.0f};

	while (glContext.is_open()) {
		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		diffuseTex.bind();
		specularTex.bind();

		lightPos.x = gsl::narrow_cast<float>(sin(glfwGetTime()) * 5.0);
		lightPos.z = gsl::narrow_cast<float>(cos(glfwGetTime()) * 15.0);

		litShader.bind();

		litShader.set("view", cam.cam().view());
		litShader.set("projection", cam.cam().projection());
		litShader.set("light.position", lightPos);
		litShader.set("camPos", cam.cam().get_pos());

		glm::mat4 model{1.0};
		model = glm::scale(model, glm::vec3(modelScale));
		litShader.set("model", model);
		glm::mat3 normalMatrix{glm::transpose(glm::inverse(model))};
		litShader.set("normalMatrix", normalMatrix);

		litShader.set("pointLights[0].position", lightPos);

		monkey.draw();

		diffuseTex.unbind();
		specularTex.unbind();

		lightShader.bind();
		lightShader.set("view", cam.cam().view());
		lightShader.set("projection", cam.cam().projection());

		model = glm::mat4{1.0f};
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.35f));
		lightShader.set("model", model);

		lightMesh.draw();

		gui::new_frame();

		ImGui::SetNextWindowSize({400, -1});
		ImGui::Begin("Model Settings", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::Text("Scale");
		ImGui::SliderFloat("Scale", &modelScale, 0.01f, 100.f);
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
	ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	if (ImGui::Button("Open file browser")) {
		fileBrowser.Open();
	}
	ImGui::End();
}
