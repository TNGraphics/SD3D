//
// Created by Tobias on 9/28/2020.
//

#ifndef SD3D_TEXTURE_H
#define SD3D_TEXTURE_H

#include <glad/glad.h>

class Texture
{
public:
	struct Settings
	{
		GLenum format{GL_RGB};
		GLint wrapS{GL_REPEAT};
		GLint wrapT{GL_REPEAT};
		GLint minFilter{GL_LINEAR};
		GLint magFilter{GL_LINEAR};
	};

private:
	// TODO handle texture slots
	GLuint m_id{};

	int m_width{};
	int m_height{};
	int m_nrChannels{};

public:
	// TODO different modes for texture -> texture wrapping, etc.
	[[maybe_unused]] Texture(const char *path, const Settings &settings);

	[[maybe_unused]] explicit Texture(const char *path) : Texture(path, Settings{})
	{}

	// TODO is this a good idea?
	Texture(const Texture &other);

	Texture(Texture &&) = delete;

	[[maybe_unused]] void bind() const
	{ glBindTexture(GL_TEXTURE_2D, m_id); }

	[[nodiscard]] GLuint get_id() const
	{ return m_id; }

	[[nodiscard]] int get_width() const
	{ return m_width; }

	[[nodiscard]] int get_height() const
	{ return m_height; }

	[[nodiscard]] int get_nr_channels() const
	{ return m_nrChannels; }

};


#endif //SD3D_TEXTURE_H
