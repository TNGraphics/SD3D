//
// Created by Tobias on 9/22/2020.
//

#ifndef SD3D_SHADER_H
#define SD3D_SHADER_H

#include <glad/glad.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(pop)

#include <string>

class Shader {
private:
	GLuint m_id;

	static std::string read_file_contents(const char *path);

	static int check_shader_error(GLuint shaderId);

	static int check_program_error(GLuint programId);

public:
	Shader();
	Shader(const char *vertexPath, const char *fragmentPath);
	Shader(std::string_view vertexPath, std::string_view fragmentPath);
	~Shader();

	Shader(const Shader &) = delete;
	Shader &operator=(const Shader &) = delete;

	Shader(Shader &&) noexcept;
	Shader &operator=(Shader &&) noexcept;

	[[maybe_unused]] void use() const;

	[[nodiscard]] GLuint get_id() const { return m_id; }

	[[maybe_unused]] void set(const char *name, bool val) const {
		glUniform1i(glGetUniformLocation(m_id, name), val);
	}

	[[maybe_unused]] void set(const char *name, int val) const {
		glUniform1i(glGetUniformLocation(m_id, name), val);
	}

	[[maybe_unused]] void set(const char *name, float val) const {
		glUniform1f(glGetUniformLocation(m_id, name), val);
	}

	[[maybe_unused]] void set(const char *name, double val) const {
		glUniform1d(glGetUniformLocation(m_id, name), val);
	}

	[[maybe_unused]] void set(const char *name, glm::mat2 val) const {
		glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, GL_FALSE,
						   glm::value_ptr(val));
	}

	[[maybe_unused]] void set(const char *name, glm::mat3 val) const {
		glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE,
						   glm::value_ptr(val));
	}

	[[maybe_unused]] void set(const char *name, glm::mat4 val) const {
		glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE,
						   glm::value_ptr(val));
	}
};

#endif // SD3D_SHADER_H
