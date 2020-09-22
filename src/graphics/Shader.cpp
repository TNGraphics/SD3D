//
// Created by Tobias on 9/22/2020.
//

#include <iostream>
#include <sstream>
#include <fstream>

#include "Shader.h"

char Shader::errorLogBuffer[512];

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
	auto vertexSource{read_file_contents(vertexPath)};
	auto fragmentSource{read_file_contents(fragmentPath)};

	// TODO maybe there is a better way
	const auto *vertexSourceCSTR{vertexSource.c_str()};
	const auto *fragmentSourceCSTR{fragmentSource.c_str()};

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSourceCSTR, nullptr);
	glCompileShader(vertexShader);
	if(!check_shader_error(vertexShader)) std::cout << "VERTEX STAGE\n";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSourceCSTR, nullptr);
	glCompileShader(fragmentShader);
	if(!check_shader_error(fragmentShader)) std::cout << "FRAGMENT STAGE\n";

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	check_program_error(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

[[maybe_unused]] void Shader::use() const
{
	glUseProgram(ID);
}

std::string Shader::read_file_contents(const char *path)
{
	std::stringstream ret{};
	std::ifstream shaderFile;
	try
	{
		shaderFile.open(path);
		ret << shaderFile.rdbuf();
		shaderFile.close();
	}
	catch(const std::ifstream::failure& e)
	{
		std::cout << "ERROR when opening file\n" << e.what() << '\n';
	}
	return ret.str();
}

int Shader::check_shader_error(GLuint shaderID)
{
	int success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shaderID, 512, nullptr, errorLogBuffer);
		std::cout << "SHADER STAGE ERROR!\n" << errorLogBuffer << '\n';
	}
	return success;
}

int Shader::check_program_error(GLuint programID)
{
	int success;
	glGetShaderiv(programID, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(programID, 512, nullptr, errorLogBuffer);
		std::cout << "SHADER PROGRAM ERROR!\n" << errorLogBuffer << '\n';
	}
	return success;
}
