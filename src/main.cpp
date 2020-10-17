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

#include "graphics/ImGuiHandler.h"

#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/data/GlMesh.h"
#include "graphics/data/Model.h"

#include "models/primitives.h"

#include "controls/GeneralInputHandler.h"
#include "controls/OrbitCameraController.h"

void framebuffer_size_callback(GLFWwindow *, int width, int height);

void GLAPIENTRY opengl_message_callback(GLenum source, GLenum type, GLuint id,
										GLenum severity, GLsizei length,
										const GLchar *message,
										const void *userParam);

void fps_gui();

float g_mixVal{};

int main(int argc, const char *argv[]) {
	using namespace sd3d;
	spdlog::set_level(spdlog::level::debug);

	std::string resPath{};
	bool showHelp = false;
	int width{600};
	int height{600};
	bool noVsync{};
	std::string file{};
	auto cli =
		lyra::opt(resPath, "res-path")["-r"]["--res"]["--path"]["-p"](
			"The path of the res folder relative to the executable") |
		lyra::opt(width, "width")["-w"]["--width"]("The window width") |
		lyra::opt(height, "height")["-h"]["--height"]("The window height") |
		lyra::opt(noVsync)["--disable-vsync"]("Force VSYNC to be disabled") |
		lyra::opt(file, "file")["-f"]["--file"]("File to open") |
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

	const double aspect{static_cast<double>(width) /
						static_cast<double>(height)};

	if (noVsync) {
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	} else {
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	}

	stbi_set_flip_vertically_on_load(true);

	// initialize OpenGL in the correct version (4.6)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window and check if it worked
	GLFWwindow *window =
		glfwCreateWindow(width, height, "First Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//	glfwSetWindowAspectRatio(window, 16, 9);

	// Try to load the OpenGL functions and fail if it didn't work
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the viewport and the callback for resizing
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (noVsync) {
		glfwSwapInterval(0);
	} else {
		glfwSwapInterval(1);
	}

	gui::setup_imgui(window);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

	GeneralInputHandler inputHandler{window};

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_message_callback, nullptr);

	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	Shader shader{resPath + "shaders/builtin/generic.vert",
				  resPath + "shaders/builtin/generic.frag"};

	Texture container{resPath + "img/container.jpg"};

	auto monkey{
		Model::from_path(resPath + (file.empty() ? "/models/cube.fbx" : file))};

	OrbitCameraController cam{
		{70.0, aspect, glm::vec3{0, 0, -10}},
		{glm::vec3{}, 10.0f, 2.5f, 20.0f, 1.f, 0.25f, 3.f}};

	shader.use();
	shader.set("texture1", 0);

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

		shader.use();
		shader.set("mixVal", g_mixVal);
		shader.set("view", cam.cam().view());
		shader.set("projection", cam.cam().projection());

		glm::mat4 model{1.0};
		shader.set("model", model);

		monkey.draw();

		gui::new_frame();

		// Do IMGUI stuff here
		fps_gui();

		gui::render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		inputHandler.update();

		if (!gui::get_io().WantCaptureMouse &&
			inputHandler.is_mouse_pressed()) {
			cam.rotate(inputHandler.d_x(), inputHandler.d_y());
		}
		cam.zoom(inputHandler.d_scroll());
		cam.update(gsl::narrow_cast<float>(deltaTime));
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

void GLAPIENTRY opengl_message_callback(
	[[maybe_unused]] GLenum source, GLenum type, [[maybe_unused]] GLuint id,
	GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message,
	[[maybe_unused]] const void *userParam) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		spdlog::error("GL ERROR: type {} message: {}", type, message);
	} else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
		spdlog::warn("GL WARNING: type {} message: {}", type, message);
	}
}
