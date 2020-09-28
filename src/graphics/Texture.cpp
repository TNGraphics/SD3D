//
// Created by Tobias on 9/28/2020.
//

#include <stb_image.h>
#include <iostream>
#include "Texture.h"

Texture::Texture(const char *path, const Settings& settings)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);

	unsigned  char *data = stbi_load(path, &m_width, &m_height, &m_nrChannels, 0);
	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, settings.format, m_width, m_height, 0, settings.format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load image data\n";
	stbi_image_free(data);
}

Texture::Texture(const Texture &other)
{
	m_id = other.m_id;
	m_width = other.m_width;
	m_height = other.m_height;
	m_nrChannels = other.m_nrChannels;
}
