//
// Created by Tobias on 9/28/2020.
//
#include <stb_image.h>
#include <algorithm>
#include <iostream>
#include <spdlog/spdlog.h>

#include "Texture.h"

Texture::Texture(Texture &&other) noexcept :
	m_id{other.m_id},
	m_width{other.m_width},
	m_height{other.m_height},
	m_nrChannels{other.m_nrChannels} {
	other.m_id = 0;
	other.m_width = 0;
	other.m_height = 0;
	other.m_nrChannels = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
	m_id = other.m_id;
	m_width = other.m_width;
	m_height = other.m_height;
	m_nrChannels = other.m_nrChannels;
	other.m_id = 0;
	other.m_width = 0;
	other.m_height = 0;
	other.m_nrChannels = 0;
	return *this;
}

Texture::~Texture() {
	glDeleteTextures(1, &m_id);
}

Texture::Texture(const char *path, const Settings &settings, GLenum slot) :
	m_slot{validate_slot(slot)} {
	spdlog::info("Loading image from path: {}", path);
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);

	unsigned char *data =
		stbi_load(path, &m_width, &m_height, &m_nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, settings.format, m_width, m_height, 0,
					 settings.format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load image data\n";
	stbi_image_free(data);
}

const Texture::Settings &Texture::png_settings() {
	static Settings s{.format = GL_RGBA};
	return s;
}

const Texture::Settings &Texture::jpg_settings() {
	static Settings s{.format = GL_RGBA};
	return s;
}
