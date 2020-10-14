//
// Created by Tobias on 9/22/2020.
//

#include <iostream>
#include <sstream>
#include <fstream>

#include <spdlog/spdlog.h>

#include "Shader.h"


Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	auto vertexSource{read_file_contents(vertexPath)};
	auto fragmentSource{read_file_contents(fragmentPath)};

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

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath) :
		Shader(vertexPath.data(), fragmentPath.data()) {}

[[maybe_unused]] void Shader::use() const {
	glUseProgram(m_id);
}

std::string Shader::read_file_contents(const char *path) {
	std::stringstream ret{};
	std::ifstream shaderFile;
	try {
		shaderFile.open(path);
		ret << shaderFile.rdbuf();
		shaderFile.close();
	}
	catch (const std::ifstream::failure &e) {
		std::cout << "ERROR when opening file\n" << e.what() << '\n';
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
