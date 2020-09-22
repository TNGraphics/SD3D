//
// Created by Tobias on 9/22/2020.
//

#ifndef SD3D_SHADER_H
#define SD3D_SHADER_H

#include <glad/glad.h>
#include <string>


class Shader
{
private:
	GLuint ID;

	static std::string read_file_contents(const char* path);

	static char errorLogBuffer[];
	static int check_shader_error(GLuint shaderID);
	static int check_program_error(GLuint programID);

public:
	// TODO maybe destructor with glDeleteShader ?+
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const Shader& other)
	{ ID = other.ID; }
	Shader(Shader&&) = delete;

	[[maybe_unused]] void use() const;

	[[nodiscard]] GLuint get_ID() const
	{ return ID; }
};


#endif //SD3D_SHADER_H
