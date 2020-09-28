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
	GLuint m_id;

	static std::string read_file_contents(const char* path);

	static char errorLogBuffer[];
	static int check_shader_error(GLuint shaderId);
	static int check_program_error(GLuint programId);

public:
	// TODO maybe destructor with glDeleteShader ?+
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const Shader& other)
	{ m_id = other.m_id; }
	Shader(Shader&&) = delete;

	[[maybe_unused]] void use() const;

	[[nodiscard]] GLuint get_id() const
	{ return m_id; }

	[[maybe_unused]] void set(const char *name, bool val) const
	{ glUniform1i(glGetUniformLocation(m_id, name), val); }
	[[maybe_unused]] void set(const char *name, int val) const
	{ glUniform1i(glGetUniformLocation(m_id, name), val); }
	[[maybe_unused]] void set(const char *name, float val) const
	{ glUniform1f(glGetUniformLocation(m_id, name), val); }
	[[maybe_unused]] void set(const char *name, double val) const
	{ glUniform1d(glGetUniformLocation(m_id, name), val); }
};


#endif //SD3D_SHADER_H
