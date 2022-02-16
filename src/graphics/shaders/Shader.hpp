//
// Created by Tobias on 9/22/2020.
//

#ifndef SD3D_SHADER_H
#define SD3D_SHADER_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include <optional>
#include <string>

#include "../memory/gl_memory.hpp"

#include "ShaderHandle.hpp"

class Shader {
private:
	sd3d::memory::shared_prog_t m_id;

	static std::optional<std::string> read_file_contents(const char *path);

	static int check_shader_error(GLuint shaderId);

	static int check_program_error(GLuint programId);

protected:
	Shader(const char *vertexSource, const char *fragmentSource);
	explicit Shader(ShaderHandle vertex, ShaderHandle fragment);

	GLint get_uniform_loc(const char *name) const;

	// TODO test both

	/// Compiles a shader with the given glsl source code.
	/// \attention No new program is created (gl object name), instead the old
	/// name is used, so all shaders relying on that name will also change
	/// \param vertexSource The vertex shader source code
	/// \param fragmentSource The fragment shader source code
	void compile(const char *vertexSource, const char *fragmentSource);

	void compile(ShaderHandle vertex, ShaderHandle fragment);

	/// Same as \ref compile but creates a new object name so other shaders are not affected
	/// \param vertexSource The vertex shader source code
	/// \param fragmentSource The fragment shader source code
	void recompile(const char *vertexSource, const char *fragmentSource);

	void recompile(ShaderHandle vertex, ShaderHandle fragment);

public:
	Shader();
	Shader(std::string_view vertexPath, std::string_view fragmentPath);
	virtual ~Shader() = default;

	Shader(const Shader &) = default;
	Shader &operator=(const Shader &);

	Shader(Shader &&) noexcept;
	Shader &operator=(Shader &&) noexcept;

	[[maybe_unused]] void bind() const;
	[[maybe_unused]] static void unbind();

	[[maybe_unused]] void set(const char *name, bool val) const;

	[[maybe_unused]] void set(const char *name, int val) const;

	[[maybe_unused]] void set_int(const char *name, int val) const;

	[[maybe_unused]] void set(const char *name, unsigned int val) const;

	[[maybe_unused]] void set(const char *name, float val) const;

	[[maybe_unused]] void set(const char *name, const glm::vec2 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y) const;

	[[maybe_unused]] void set(const char *name, const glm::vec3 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y,
							  float z) const;

	[[maybe_unused]] void set(const char *name, const glm::vec4 &val) const;

	[[maybe_unused]] void set(const char *name, float x, float y, float z,
							  float w) const;

	[[maybe_unused]] void set(const char *name, const glm::mat2 &val) const;

	[[maybe_unused]] void set(const char *name, const glm::mat3 &val) const;

	[[maybe_unused]] void set(const char *name, const glm::mat4 &val) const;

protected:
	// TODO for more functions
	[[maybe_unused]] static void set(GLint loc, float val);

	[[maybe_unused]] static void set(GLint loc, const glm::vec2 &val);

	[[maybe_unused]] static void set(GLint loc, const glm::vec3 &val);

	[[maybe_unused]] static void set(GLint loc, const glm::vec4 &val);

	[[maybe_unused]] static void set(GLint loc, const glm::mat2 &val);

	[[maybe_unused]] static void set(GLint loc, const glm::mat3 &val);

	[[maybe_unused]] static void set(GLint loc, const glm::mat4 &val);
};

#endif // SD3D_SHADER_H
