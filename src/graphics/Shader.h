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

	[[maybe_unused]] void set(const char *name, bool val) const
	{ glUniform1i(glGetUniformLocation(ID, name), val); }
	[[maybe_unused]] void set(const char *name, int val) const
	{ glUniform1i(glGetUniformLocation(ID, name), val); }
	[[maybe_unused]] void set(const char *name, float val) const
	{ glUniform1f(glGetUniformLocation(ID, name), val); }
	[[maybe_unused]] void set(const char *name, double val) const
	{ glUniform1d(glGetUniformLocation(ID, name), val); }
};


#endif //SD3D_SHADER_H
