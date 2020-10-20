//
// Created by Tobias on 9/22/2020.
//

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#pragma warning(push, 0)

#include <glm/gtc/type_ptr.hpp>

#pragma warning(pop)

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

Shader::Shader(const char *vertexPath, const char *fragmentPath) : m_id{sd3d::memory::create_prog()}{
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

	glAttachShader(*m_id, vertexShader);
	glAttachShader(*m_id, fragmentShader);
	glLinkProgram(*m_id);

	check_program_error(*m_id);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(Shader &&other) noexcept : m_id{std::move(other.m_id)} {
	other.m_id = nullptr;
}

Shader &Shader::operator=(Shader &&other) noexcept {
	m_id = std::move(other.m_id);
	other.m_id = nullptr;
	return *this;
}

Shader &Shader::operator=(const Shader &other) {
	if(this != &other) {
		m_id = other.m_id;
	}
	return *this;
}

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath) :
	Shader(vertexPath.data(), fragmentPath.data()) {}

[[maybe_unused]] void Shader::bind() const {
	glUseProgram(*m_id);
}

void Shader::unbind() {
	glUseProgram(0);
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

GLint Shader::get_uniform_loc(const GLchar *name) const {
	return glGetUniformLocation(*m_id, name);
}

void Shader::set(const char *name, bool val) const {
	glUniform1i(get_uniform_loc(name), val);
}

void Shader::set(const char *name, int val) const {
	glUniform1i(get_uniform_loc(name), val);
}

void Shader::set_int(const char *name, int val) const {
	set(name, val);
}

void Shader::set(const char *name, unsigned int val) const {
	glUniform1ui(get_uniform_loc(name), val);
}

void Shader::set(const char *name, float val) const {
	glUniform1f(get_uniform_loc(name), val);
}

void Shader::set(const char *name, const glm::vec2 &val) const {
	glUniform2fv(get_uniform_loc(name), 1, glm::value_ptr(val));
}

void Shader::set(const char *name, float x, float y) const {
	glUniform2f(get_uniform_loc(name), x, y);
}

void Shader::set(const char *name, const glm::vec3 &val) const {
	glUniform3fv(get_uniform_loc(name), 1, glm::value_ptr(val));
}

void Shader::set(const char *name, float x, float y, float z) const {
	glUniform3f(get_uniform_loc(name), x, y, z);
}

void Shader::set(const char *name, const glm::vec4 &val) const {
	glUniform4fv(get_uniform_loc(name), 1, glm::value_ptr(val));
}

void Shader::set(const char *name, float x, float y, float z, float w) const {
	glUniform4f(get_uniform_loc(name), x, y, z, w);
}

void Shader::set(const char *name, const glm::mat2 &val) const {
	glUniformMatrix2fv(get_uniform_loc(name), 1, GL_FALSE,
					   glm::value_ptr(val));
}

void Shader::set(const char *name, const glm::mat3 &val) const {
	glUniformMatrix3fv(get_uniform_loc(name), 1, GL_FALSE,
					   glm::value_ptr(val));
}

void Shader::set(const char *name, const glm::mat4 &val) const {
	glUniformMatrix4fv(get_uniform_loc(name), 1, GL_FALSE,
					   glm::value_ptr(val));
}
