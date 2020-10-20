//
// Created by Tobias on 9/28/2020.
//
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <algorithm>
#include <iostream>

#include <glad/glad.h>

#include "Texture.h"

Texture::Texture(Texture &&other) noexcept :
	m_id{other.m_id},
	m_width{other.m_width},
	m_height{other.m_height},
	m_slot{other.m_slot},
	m_type{other.m_type} {
	other.m_id = 0;
	other.m_width = 0;
	other.m_height = 0;
	other.m_slot = GL_TEXTURE0;
	other.m_type = Type::DIFFUSE;
}

Texture &Texture::operator=(Texture &&other) noexcept {
	m_id = other.m_id;
	m_width = other.m_width;
	m_height = other.m_height;
	m_slot = other.m_slot;
	m_type = other.m_type;
	other.m_id = 0;
	other.m_width = 0;
	other.m_height = 0;
	other.m_slot = GL_TEXTURE0;
	other.m_type = Type::DIFFUSE;
	return *this;
}

Texture::Texture(const char *path, const Settings &settings, GLenum slot,
				 Type type) :
	m_slot{validate_slot(slot)},
	m_type{type},
	m_id{sd3d::memory::create_tex()} {
	spdlog::info("Loading image from path: {}", path);
	glBindTexture(GL_TEXTURE_2D, *m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);

	int nrChannels{0};

	unsigned char *data = stbi_load(path, &m_width, &m_height, &nrChannels, 0);
	if (data) {
		auto format = num_channels_to_format(nrChannels);
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format,
					 GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load image data\n";
	stbi_image_free(data);
}

// region additional constructors
Texture::Texture(std::string_view path, const Texture::Settings &settings,
				 Texture::Type type) :
	Texture{path.data(), settings, GL_TEXTURE0, type} {}

Texture::Texture(const char *path, const Texture::Settings &settings,
				 Texture::Type type) :
	Texture{path, settings, GL_TEXTURE0, type} {}

Texture::Texture(std::string_view path, const Texture::Settings &settings,
				 GLenum slot, Texture::Type type) :
	Texture{path.data(), settings, slot, type} {}

Texture::Texture(const char *path, Texture::Type type) :
	Texture(path, Settings{}, type) {}

Texture::Texture(std::string_view path, Texture::Type type) :
	Texture(path, Settings{}, type) {}

Texture::Texture(const char *path, GLenum slot, Texture::Type type) :
	Texture(path, Settings{}, slot, type) {}

Texture::Texture(std::string_view path, GLenum slot, Texture::Type type) :
	Texture(path, Settings{}, slot, type) {}
// endregion additional constructors

Texture &Texture::operator=(const Texture &other) {
	if (this != &other) {
		m_id = other.m_id;
		m_width = other.m_width;
		m_height = other.m_height;
		m_slot = other.m_slot;
		m_type = other.m_type;
	}
	return *this;
}

GLenum Texture::num_channels_to_format(int nrChannels) {
	if (nrChannels == 3) {
		return GL_RGB;
	} else if (nrChannels == 4) {
		return GL_RGBA;
	} else {
		spdlog::error("Image format with {} channels not supported!",
					  nrChannels);
		return GL_INVALID_ENUM;
	}
}

void Texture::bind_unchecked(GLenum slot, GLuint id) {
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, id);
	// TODO check if unbind is needed
}

GLenum Texture::validate_slot(GLenum slot) {
	return std::clamp(
		slot, static_cast<GLenum>(GL_TEXTURE0),
		static_cast<GLenum>(GL_TEXTURE0 + GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS -
							1));
}

void Texture::bind() const {
	bind_unchecked(m_slot, *m_id);
}

void Texture::bind_to_num(unsigned int num) const {
	bind(GL_TEXTURE0 + num);
}

void Texture::bind(GLenum slot) const {
	bind(slot, *m_id);
}

void Texture::bind(GLenum slot, GLuint textureId) {
	bind_unchecked(validate_slot(slot), textureId);
}

void Texture::unbind() const {
	bind_unchecked(m_slot, 0);
}

void Texture::unbind(GLenum slot) {
	bind(slot, 0);
}

void Texture::unbind_num(unsigned int num) {
	unbind(GL_TEXTURE0 + num);
}

void Texture::reset() {
	glActiveTexture(GL_TEXTURE0);
}

void Texture::change_slot(GLenum slot) {
	m_slot = validate_slot(slot);
}

int Texture::get_width() const {
	return m_width;
}

int Texture::get_height() const {
	return m_height;
}

Texture::Type Texture::get_type() const {
	return m_type;
}
