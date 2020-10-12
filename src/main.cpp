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

#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "controls/OrbitCameraController.h"
#include "controls/GeneralInputHandler.h"

void framebuffer_size_callback(GLFWwindow *, int width, int height);

void process_input(GLFWwindow *window);

void process_mouse_input(GLFWwindow *, int button, int action, int mods);

// TODO use NanoGUI for gui

float g_mixVal{};
bool g_mousePressed{};

int main(int argc, const char *argv[]) {
	std::string resPath{};
	bool showHelp = false;
	auto cli = lyra::opt(resPath, "res-path")["-r"]["--res"]["--path"]["-p"](
			"The path of the res folder relative to the executable")
			   | lyra::help(showHelp);

	auto parsed = cli.parse({argc, argv});

	if(!parsed) {
		spdlog::error("CLI argument parsing error: {}", parsed.errorMessage());
		return 1;
	}

	if(showHelp) {
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

	// Try to load the OpenGL functions and fail if it didn't work
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the viewport and the callback for resizing
	glViewport(0, 0, 600, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, process_mouse_input);

	// temp vertex data (2 cute lil triangles)
	float vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f, 3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f, 2.0f, -2.5f),
			glm::vec3(1.5f, 0.2f, -1.5f),
			glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	// EBO index data
	GLuint indices[] = {
			0, 1, 3,
			0, 2, 3
	};

	GeneralInputHandler inputHandler{window};

	Shader shader{resPath + "shaders/vert.glsl", resPath + "shaders/frag.glsl"};

	Texture container{resPath + "img/container.jpg"};
	Texture face{resPath + "img/face.png", Texture::Settings{.format = GL_RGBA}};

	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create and bind a VAO for vertex attributes
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Bind vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	OrbitCameraController cam{{70.0, 600.0 / 600.0, glm::vec3{0, 0, -10}}, glm::vec3{}, 10.0, 10};

	shader.use();
	shader.set("texture1", 0);
	shader.set("texture2", 1);

	double deltaTime;
	double lastFrame{glfwGetTime()};

	double deltaX{};
	double deltaY{};

	double mX{};
	double mY{};
	double lastX{};
	double lastY{};

	bool first{true};
	while (!glfwWindowShouldClose(window)) {
		glfwGetCursorPos(window, &mX, &mY);
		if (first) {
			lastX = mX;
			lastY = mY;
			first = false;
		}
		deltaX = mX - lastX;
		deltaY = mY - lastY;

		lastX = mX;
		lastY = mY;

		auto currentFrame{glfwGetTime()};
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

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
		glBindVertexArray(vao);

		for (const auto &cubePosition : cubePositions) {
			glm::mat4 model{1.0};
			model = glm::translate(model, cubePosition);
//			model = glm::rotate(model, (float) glfwGetTime(), glm::vec3{1.0, 0.3, 0.5});
			shader.set("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if(g_mousePressed) {
			cam.move(deltaX, deltaY);
		}
		cam.update(static_cast<float>(deltaTime));
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		g_mixVal += 0.1f;
		if (g_mixVal > 1.0f) g_mixVal = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		g_mixVal -= 0.1f;
		if (g_mixVal < 0.0f) g_mixVal = 0.0f;
	}
}

void process_mouse_input(GLFWwindow *, int button, int action, [[maybe_unused]] int mods) {
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		g_mousePressed = true;
	} else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		g_mousePressed = false;
	}
}
