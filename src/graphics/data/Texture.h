//
// Created by Tobias on 9/28/2020.
//

#ifndef SD3D_TEXTURE_H
#define SD3D_TEXTURE_H

#include <string_view>

#include <glad/glad.h>

class Texture {
	// TODO some way to delete the texture from GPU memory
public:
	struct Settings {
		GLenum format{GL_RGB};
		GLint wrapS{GL_REPEAT};
		GLint wrapT{GL_REPEAT};
		GLint minFilter{GL_LINEAR};
		GLint magFilter{GL_LINEAR};
	};

	static const Settings &png_settings();
	static const Settings &jpg_settings();

private:
	// TODO handle texture slots
	GLuint m_id{};

	int m_width{};
	int m_height{};
	int m_nrChannels{};
	GLenum m_slot{GL_TEXTURE0};

	static constexpr GLenum validate_slot(GLenum slot) {
		return std::clamp(
			slot, static_cast<GLenum>(GL_TEXTURE0),
			static_cast<GLenum>(GL_TEXTURE0 +
								GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1));
	}

	static void bind_unchecked(GLenum slot, GLuint id) {
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, id);
		// TODO check if unbind is needed
	}

public:
	[[maybe_unused]] Texture(const char *path, const Settings &settings,
							 GLenum slot = GL_TEXTURE0);

	[[maybe_unused]] Texture(std::string_view path, const Settings &settings,
							 GLenum slot = GL_TEXTURE0) :
		Texture{path.data(), settings, slot} {}

	[[maybe_unused]] explicit Texture(const char *path) :
		Texture(path, Settings{}) {}

	[[maybe_unused]] explicit Texture(std::string_view path) :
		Texture(path, Settings{}) {}

	[[maybe_unused]] explicit Texture(const char *path, GLenum slot) :
		Texture(path, Settings{}, slot) {}

	[[maybe_unused]] explicit Texture(std::string_view path, GLenum slot) :
		Texture(path, Settings{}, slot) {}

	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;

	Texture(Texture &&) noexcept;
	Texture &operator=(Texture &&) noexcept;

	~Texture();

	[[maybe_unused]] void bind_to_num(unsigned int num) const {
		bind(GL_TEXTURE0 + num);
	}
	[[maybe_unused]] void bind(GLenum slot) const { bind(slot, m_id); }
	[[maybe_unused]] void bind() const { bind_unchecked(m_slot, m_id); }

	[[maybe_unused]] void unbind() const { bind_unchecked(m_slot, 0); }

	[[maybe_unused]] static void unbind(GLenum slot) { bind(slot, 0); }

	static void bind(GLenum slot, GLuint textureId) {
		bind_unchecked(validate_slot(slot), textureId);
	}

	[[maybe_unused]] [[nodiscard]] GLuint get_id() const { return m_id; }

	[[maybe_unused]] [[nodiscard]] int get_width() const { return m_width; }

	[[maybe_unused]] [[nodiscard]] int get_height() const { return m_height; }

	[[maybe_unused]] [[nodiscard]] int get_nr_channels() const {
		return m_nrChannels;
	}

	[[maybe_unused]] void change_slot(GLenum slot) {
		m_slot = validate_slot(slot);
	}
};

#endif // SD3D_TEXTURE_H
