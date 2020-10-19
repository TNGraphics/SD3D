//
// Created by Tobias on 9/22/2020.
//

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include <spdlog/spdlog.h>

#include "Shader.h"

// region shader source
// TODO move to separate file
// maybe include shader texts at compile time?
const std::string &default_vertex_source() {
	// TODO add vertex color support
	static std::string source = R"VERT(#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = aTexCoord;
})VERT";
	return source;
}

const std::string &default_fragment_source() {
	static std::string source = R"FRAG(#version 460 core
out vec4 FragColor;

in vec2 texCoord;

void main()
{
    FragColor = vec4(1.0);
})FRAG";
	return source;
}
// endregion shader source

Shader::Shader() : Shader("", "") {}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	auto vertexSource{
		read_file_contents(vertexPath).value_or(default_vertex_source())};
	auto fragmentSource{
		read_file_contents(fragmentPath).value_or(default_fragment_source())};

	// TODO maybe there is a better way
	const auto *vertexSourceCstr{vertexSource.c_str()};
	const auto *fragmentSourceCstr{fragmentSource.c_str()};

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSourceCstr, nullptr);
	glCompileShader(vertexShader);
	if (!check_shader_error(vertexShader)) std::cout << "VERTEX STAGE\n";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSourceCstr, nullptr);
	glCompileShader(fragmentShader);
	if (!check_shader_error(fragmentShader)) std::cout << "FRAGMENT STAGE\n";

	m_id = glCreateProgram();
	glAttachShader(m_id, vertexShader);
	glAttachShader(m_id, fragmentShader);
	glLinkProgram(m_id);

	check_program_error(m_id);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(m_id);
}

Shader::Shader(Shader &&other) noexcept : m_id{other.m_id} {
	other.m_id = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
	m_id = other.m_id;
	other.m_id = 0;
	return *this;
}

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath) :
	Shader(vertexPath.data(), fragmentPath.data()) {}

[[maybe_unused]] void Shader::use() const {
	glUseProgram(m_id);
}

std::optional<std::string> Shader::read_file_contents(const char *path) {
	// TODO when file could not be opened, return default shader
	std::stringstream ret{};
	std::ifstream shaderFile(path);
	if (!shaderFile.good()) {
		return {};
	}
	try {
		ret << shaderFile.rdbuf();
		shaderFile.close();
	} catch (const std::ifstream::failure &e) {
		std::cout << "ERROR when opening file\n" << e.what() << '\n';
		return {};
	}
	return ret.str();
}

int Shader::check_shader_error(GLuint shaderId) {
	static char logBuffer[512];
	int success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderId, 512, nullptr, logBuffer);
		std::cout << "SHADER STAGE ERROR!\n" << logBuffer << '\n';
	}
	return success;
}

int Shader::check_program_error(GLuint programId) {
	static char logBuffer[512];
	int success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, nullptr, logBuffer);
		std::cout << "SHADER PROGRAM ERROR!\n" << logBuffer << '\n';
	}
	return success;
}
