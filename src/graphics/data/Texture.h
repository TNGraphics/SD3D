//
// Created by Tobias on 9/28/2020.
//

#ifndef SD3D_TEXTURE_H
#define SD3D_TEXTURE_H

#include <string_view>

#include <GLFW/glfw3.h>

#include "../memory/gl_memory.h"

class Texture {
	// TODO some way to delete the texture from GPU memory
public:
	struct Settings {
		GLint wrapS{GL_REPEAT};
		GLint wrapT{GL_REPEAT};
		GLint minFilter{GL_LINEAR};
		GLint magFilter{GL_LINEAR};
	};

	enum class Type { DIFFUSE, SPECULAR };

private:
	// TODO handle texture slots
	sd3d::memory::shared_tex_t m_id{};

	int m_width{};
	int m_height{};
	GLenum m_slot;

	Type m_type{Type::DIFFUSE};

	static GLenum validate_slot(GLenum slot);

	static void bind_unchecked(GLenum slot, GLuint id);

	static GLenum num_channels_to_format(int nrChannels);

public:
	[[maybe_unused]] Texture(const char *path, const Settings &settings,
							 GLenum slot, Type type = Type::DIFFUSE);

	[[maybe_unused]] Texture(std::string_view path, const Settings &settings,
							 GLenum slot, Type type = Type::DIFFUSE);

	[[maybe_unused]] Texture(const char *path, const Settings &settings,
							 Type type = Type::DIFFUSE);

	[[maybe_unused]] Texture(std::string_view path, const Settings &settings,
							 Type type = Type::DIFFUSE);

	[[maybe_unused]] explicit Texture(const char *path,
									  Type type = Type::DIFFUSE);

	[[maybe_unused]] explicit Texture(std::string_view path,
									  Type type = Type::DIFFUSE);

	[[maybe_unused]] Texture(const char *path, GLenum slot,
									  Type type = Type::DIFFUSE);

	[[maybe_unused]] Texture(std::string_view path, GLenum slot,
									  Type type = Type::DIFFUSE);

	Texture(const Texture &) = default;
	Texture &operator=(const Texture &);

	Texture(Texture &&) noexcept;
	Texture &operator=(Texture &&) noexcept;

	[[maybe_unused]] void bind() const;
	[[maybe_unused]] void bind_to_num(unsigned int num) const;
	[[maybe_unused]] void bind(GLenum slot) const;
	[[maybe_unused]] static void bind(GLenum slot, GLuint textureId);

	[[maybe_unused]] void unbind() const;

	[[maybe_unused]] static void unbind(GLenum slot);
	[[maybe_unused]] static void unbind_num(unsigned int num);

	[[maybe_unused]] static void reset();

	[[maybe_unused]] void change_slot(GLenum slot);

	[[maybe_unused]] [[nodiscard]] int get_width() const;

	[[maybe_unused]] [[nodiscard]] int get_height() const;

	[[maybe_unused]] [[nodiscard]] Type get_type() const;

};

#endif // SD3D_TEXTURE_H
