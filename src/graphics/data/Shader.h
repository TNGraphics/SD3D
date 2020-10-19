//
// Created by Tobias on 9/22/2020.
//

#ifndef SD3D_SHADER_H
#define SD3D_SHADER_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include <string>
#include <optional>

class Shader {
private:
	GLuint m_id;

	static std::optional<std::string> read_file_contents(const char *path);

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

	[[maybe_unused]] [[nodiscard]] GLuint get_id() const { return m_id; }

	[[maybe_unused]] void set(const char *name, bool val) const;

	[[maybe_unused]] void set(const char *name, int val) const;

	[[maybe_unused]] void set(const char *name, unsigned int val) const;

	[[maybe_unused]] void set(const char *name, float val) const;

	[[maybe_unused]] void set(const char *name, const glm::vec2 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y) const;

	[[maybe_unused]] void set(const char *name, const glm::vec3 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y, float z) const;

	[[maybe_unused]] void set(const char *name, const glm::vec4 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y, float z, float w) const;

	[[maybe_unused]] void set(const char *name, const glm::mat2 &val) const;

	[[maybe_unused]] void set(const char *name, const glm::mat3 &val) const;

	[[maybe_unused]] void set(const char *name, const glm::mat4 &val) const;
};

#endif // SD3D_SHADER_H
