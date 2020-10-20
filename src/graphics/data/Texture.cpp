//
// Created by Tobias on 9/28/2020.
//
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <algorithm>
#include <unordered_map>
#include <utility>

#include <glad/glad.h>

#include "Texture.h"

static std::pair<bool, sd3d::memory::shared_tex_t> add_or_get(const char *path);

Texture::Texture(Texture &&other) noexcept :
	m_id{std::move(other.m_id)},
	m_slot{other.m_slot},
	m_type{other.m_type} {
	other.m_id = nullptr;
	other.m_slot = GL_TEXTURE0;
	other.m_type = Type::DIFFUSE;
}

Texture &Texture::operator=(Texture &&other) noexcept {
	m_id = std::move(other.m_id);
	m_slot = other.m_slot;
	m_type = other.m_type;
	other.m_id = nullptr;
	other.m_slot = GL_TEXTURE0;
	other.m_type = Type::DIFFUSE;
	return *this;
}

Texture::Texture(const Texture::Settings &settings, Texture::Type type) :
	m_slot{GL_TEXTURE0},
	m_type{type},
	m_id{sd3d::memory::create_tex()} {
	glBindTexture(GL_TEXTURE_2D, *m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);
}

Texture::Texture(Texture::Type type) : Texture{Settings{}, type} {}

Texture::Texture(const char *path, const Settings &settings, GLenum slot,
				 Type type) : m_slot{validate_slot(slot)}, m_type{type} {
	// TODO way to force reloading texture
	bool isNewVal{false};
	std::tie(isNewVal, m_id) = add_or_get(path);
	if(isNewVal) {
		spdlog::debug("Loading file {}", path);
		glBindTexture(GL_TEXTURE_2D, *m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						settings.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
						settings.magFilter);

		int nrChannels{0};

		int w{}, h{};
		unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
		if (data) {
			auto format = num_channels_to_format(nrChannels);
			glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
						 GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			spdlog::error("Failed to load image data from file {}", path);
		}
		stbi_image_free(data);
	} else {
		spdlog::debug("File {} already loaded, using cached version", path);
	}
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

Texture::Type Texture::get_type() const {
	return m_type;
}

Texture Texture::empty_from_pixel(GLubyte *pixel, Texture::Type type) {
	Texture t(type);
	t.bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, pixel);
	glGenerateMipmap(GL_TEXTURE_2D);
	t.unbind();
	return t;
}

Texture Texture::empty_white(Texture::Type type) {
	GLubyte pixel[3] = {0xFF, 0xFF, 0xFF};
	return empty_from_pixel(pixel, type);
}

Texture Texture::empty_black(Texture::Type type) {
	GLubyte pixel[3] = {0x0, 0x0, 0x0};
	return empty_from_pixel(pixel, type);
}

static std::pair<bool, sd3d::memory::shared_tex_t> add_or_get(const char *path) {
	using lookup_t = std::unordered_map<std::string, sd3d::memory::weak_tex_t>;
	static lookup_t lookup{};
	if(lookup.contains(path)) {
		auto temp = lookup.at(path);
		if(!temp.expired()) {
			spdlog::debug("Cached texture");
			return std::make_pair(false, lookup.at(path).lock());
		}
		// TODO maybe don't do everytime a resource is requested
		std::erase_if(lookup, [](const lookup_t::value_type &item) {
			return item.second.expired();
		});
	}
	spdlog::debug("New texture");
	auto newTex = sd3d::memory::create_tex();

	lookup.try_emplace(path, newTex);
	spdlog::debug("Before return");
	return std::make_pair(true, newTex);
}
