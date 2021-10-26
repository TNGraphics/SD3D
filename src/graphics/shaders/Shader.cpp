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

#include "builtin/builtin_shaders.hpp"

#include "Shader.hpp"

Shader::Shader(const char *vertexSource, const char *fragmentSource) : m_id{} {
	compile(vertexSource, fragmentSource);
}

Shader::Shader() : Shader("", "") {}

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath) :
	Shader{read_file_contents(vertexPath.data())
			   .value_or(sd3d::shaders::error_vertex_src())
			   .c_str(),
		   read_file_contents(fragmentPath.data())
			   .value_or(sd3d::shaders::error_fragment_src())
			   .c_str()} {}

Shader::Shader(Shader &&other) noexcept : m_id{std::move(other.m_id)} {
	other.m_id.reset();
}

Shader &Shader::operator=(Shader &&other) noexcept {
	m_id = std::move(other.m_id);
	other.m_id.reset();
	return *this;
}

Shader &Shader::operator=(const Shader &other) {
	if (this != &other) {
		m_id = other.m_id;
	}
	return *this;
}

void Shader::compile(const char *vertexSource, const char *fragmentSource) {
	// TODO better error handling
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	if (!check_shader_error(vertexShader)) std::cout << "VERTEX STAGE\n";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	if (!check_shader_error(fragmentShader)) std::cout << "FRAGMENT STAGE\n";

	glAttachShader(m_id.name(), vertexShader);
	glAttachShader(m_id.name(), fragmentShader);
	glLinkProgram(m_id.name());

	check_program_error(m_id.name());

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::recompile(const char *vertexSource, const char *fragmentSource) {
	// get a new ID, because other shader objects could rely on this one
	m_id.create_new();
	compile(vertexSource, fragmentSource);
}

[[maybe_unused]] void Shader::bind() const {
	glUseProgram(m_id.name());
}

void Shader::unbind() {
	glUseProgram(0);
}

std::optional<std::string> Shader::read_file_contents(const char *path) {
	std::stringstream ret{};
	std::ifstream shaderFile(path);
	if (!shaderFile.good()) {
		spdlog::error("Error opening file {}", path);
		return std::nullopt;
	}
	try {
		ret << shaderFile.rdbuf();
		shaderFile.close();
	} catch (const std::ifstream::failure &e) {
		spdlog::error("Error {} reading file {} ({})", e.code().value(), path, e.what());
		return std::nullopt;
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
	return glGetUniformLocation(m_id.name(), name);
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
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, const glm::vec2 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, float x, float y) const {
	glUniform2f(get_uniform_loc(name), x, y);
}

void Shader::set(const char *name, const glm::vec3 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, float x, float y, float z) const {
	glUniform3f(get_uniform_loc(name), x, y, z);
}

void Shader::set(const char *name, const glm::vec4 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, float x, float y, float z, float w) const {
	glUniform4f(get_uniform_loc(name), x, y, z, w);
}

void Shader::set(const char *name, const glm::mat2 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, const glm::mat3 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(const char *name, const glm::mat4 &val) const {
	set(get_uniform_loc(name), val);
}

void Shader::set(GLint loc, float val) {
	glUniform1f(loc, val);
}

void Shader::set(GLint loc, const glm::vec2 &val) {
	glUniform2fv(loc, 1, glm::value_ptr(val));
}

void Shader::set(GLint loc, const glm::vec3 &val) {
	glUniform3fv(loc, 1, glm::value_ptr(val));
}

void Shader::set(GLint loc, const glm::vec4 &val) {
	glUniform4fv(loc, 1, glm::value_ptr(val));
}

void Shader::set(GLint loc, const glm::mat2 &val) {
	glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::set(GLint loc, const glm::mat3 &val) {
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::set(GLint loc, const glm::mat4 &val) {
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}
