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

#include "graphics/ImGuiHandler.h"

#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/data/Model.h"

#include "misc/teapot.h"

#include "controls/OrbitCameraController.h"
#include "controls/GeneralInputHandler.h"

void framebuffer_size_callback(GLFWwindow *, int width, int height);

void fps_gui();

float g_mixVal{};

int main(int argc, const char *argv[]) {
	spdlog::set_level(spdlog::level::debug);

	std::string resPath{};
	bool showHelp = false;
	auto cli = lyra::opt(resPath, "res-path")["-r"]["--res"]["--path"]["-p"](
			"The path of the res folder relative to the executable") | lyra::help(showHelp);

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

	// initialize OpenGL in the correct version (4.6)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window and check if it worked
	GLFWwindow *window = glfwCreateWindow(600, 600, "First Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
//	glfwSetWindowAspectRatio(window, 16, 9);

	// Try to load the OpenGL functions and fail if it didn't work
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the viewport and the callback for resizing
	glViewport(0, 0, 600, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gui::setup_imgui(window);

	// temp vertex data (2 cute lil triangles)
	// no actually a cute lil cube
	float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
						0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

						-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
						0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

						-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
						-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

						0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
						0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

						-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
						0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

						-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
						0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

	glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
								 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
								 glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
								 glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
								 glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)};

	GeneralInputHandler inputHandler{window};

	Shader shader{resPath + "shaders/vert.glsl", resPath + "shaders/frag.glsl"};

	Texture container{resPath + "img/container.jpg"};
	Texture face{resPath + "img/face.png", Texture::Settings{.format = GL_RGBA}};

	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	DataLayout cubeLayout{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						  {2, DataLayout::GlType::FLOAT, GL_FALSE}};
	auto cube{Model::from_data(cubeLayout, vertices, sizeof(vertices))};
	DataLayout teapotLayout{{3, DataLayout::GlType::FLOAT, GL_FALSE}};
	auto teapot{Model::from_data(teapotLayout, models::g_teapot, sizeof(models::g_teapot))};

	OrbitCameraController cam{{70.0,        600.0 / 600.0, glm::vec3{0, 0, -10}},
							  {glm::vec3{}, 10.0f,         2.5f, 20.0f, 10.0f, 10}};

	shader.use();
	shader.set("texture1", 0);
	shader.set("texture2", 1);

	double deltaTime;
	double lastFrame{glfwGetTime()};

	while (!glfwWindowShouldClose(window)) {
		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		container.bind();
		glActiveTexture(GL_TEXTURE1);
		face.bind();

		shader.use();
		shader.set("mixVal", g_mixVal);
		shader.set("view", cam.cam().view());
		shader.set("projection", cam.cam().projection());

		for (unsigned int i = 0; const auto &cubePosition : cubePositions) {
			glm::mat4 model{1.0};
			model = glm::translate(model, cubePosition);
//			model = glm::rotate(model, (float) glfwGetTime(), glm::vec3{1.0, 0.3, 0.5});
			shader.set("model", model);

			if (i % 2 == 0) {
				cube.draw();
			} else {
				teapot.draw();
			}
			++i;
		}
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		gui::new_frame();

		// Do IMGUI stuff here
		fps_gui();

		gui::render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		inputHandler.update();

		// TODO WantCaptureMouse doesn't work without UI
		if (!gui::get_io().WantCaptureMouse && inputHandler.is_mouse_pressed()) {
//		if (inputHandler.is_mouse_pressed()) {
			cam.rotate(inputHandler.d_x(), inputHandler.d_y());
		}
		cam.zoom(inputHandler.d_scroll() * deltaTime);
		cam.update(static_cast<float>(deltaTime));
	}

	gui::shutdown();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
}

void fps_gui() {
	ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	ImGui::End();
}
