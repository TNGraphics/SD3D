//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <utility>

#include <gsl/gsl-lite.hpp>

#include "../shaders/Shader.h"
#include "DataLayout.h"

#include "GlMesh.h"

namespace mem = sd3d::memory;

const DataLayout &GlMesh::vertex_layout() {
	static DataLayout d{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{2, DataLayout::GlType::FLOAT, GL_FALSE}};
	return d;
}

GlMesh GlMesh::from_data(const DataLayout &dataLayout, const float *data,
						 GLuint amount) {
	auto vao = mem::create_vao();
	auto vbo = mem::create_vbo();
	glBindVertexArray(*vao);

	// Bind the VBO to fill it with data
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	// Fill the VBO with data
	glBufferData(GL_ARRAY_BUFFER, amount, data, GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	dataLayout.bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return GlMesh(vao, amount, vbo, nullptr, false);
}

GlMesh GlMesh::from_data(const std::vector<Vertex> &data,
						 const std::vector<GLuint> &indices) {
	auto vao = mem::create_vao();
	auto vbo = mem::create_vbo();
	auto ebo = mem::create_ebo();
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(*vao);

	// Bind the VBO to fill it with data
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	// Fill the VBO with data
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex),
				 data.data(), GL_STATIC_DRAW);

	// Bind and fill EBO with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 indices.data(), GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	vertex_layout().bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return GlMesh(vao, static_cast<GLuint>(indices.size()), vbo, ebo, true);
}

GlMesh::GlMesh(mem::shared_vao_t vao, GLuint drawCount,
			   mem::shared_vbo_t vbo, mem::shared_ebo_t ebo,
			   bool useEbo) :
	m_vao{std::move(vao)},
	m_drawCount{drawCount},
	m_vbo{std::move(vbo)},
	m_ebo{std::move(ebo)},
	m_usesEbo{useEbo},
	m_initialized{true} {}

GlMesh::GlMesh(GlMesh &&other) noexcept :
	m_vao{std::move(other.m_vao)},
	m_drawCount{other.m_drawCount},
	m_vbo{std::move(other.m_vbo)},
	m_ebo{std::move(other.m_ebo)},
	m_usesEbo{other.m_usesEbo},
	m_initialized{other.m_initialized},
	m_textures{std::move(other.m_textures)} {
	other.m_vao = nullptr;
	other.m_drawCount = 0;
	other.m_vbo = nullptr;
	other.m_ebo = nullptr;
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
}

GlMesh &GlMesh::operator=(GlMesh &&other) noexcept {
	m_vao = std::move(other.m_vao);
	m_drawCount = other.m_drawCount;
	m_vbo = std::move(other.m_vbo);
	m_ebo = std::move(other.m_ebo);
	m_usesEbo = other.m_usesEbo;
	m_initialized = other.m_initialized;
	m_textures = std::move(other.m_textures);

	other.m_vao = nullptr;
	other.m_drawCount = 0;
	other.m_vbo = nullptr;
	other.m_ebo = nullptr;
	other.m_usesEbo = false;
	other.m_initialized = false;
	other.m_textures.clear();
	return *this;
}

GlMesh &GlMesh::operator=(const GlMesh &other) {
	if(this != &other) {
		m_vao = other.m_vao;
		m_drawCount = other.m_drawCount;
		m_vbo = other.m_vbo;
		m_ebo = other.m_ebo;
		m_usesEbo = other.m_usesEbo;
		m_initialized = other.m_initialized;
		m_textures = other.m_textures;
	}
	return *this;
}

void GlMesh::draw() const {
	if (!m_initialized) return;
	// bind all textures
	for (const auto &tex : m_textures) {
		tex.bind();
	}

	draw_mesh();

	// unbind all textures
	for (const auto &tex : m_textures) {
		tex.unbind();
	}
	Texture::reset();
}

void GlMesh::draw(Shader &shader) const {
	if (!m_initialized) return;

	placeholder_tex().bind_to_num(0);

	int diffuseNr = 0;
	int specularNr = 0;
	for(unsigned int i = 1; const auto &tex : m_textures) {
		std::string property{};
		if(tex.get_type() == Texture::Type::DIFFUSE) {
			property = "material.texture_diffuse" + std::to_string(++diffuseNr);
		} else if(tex.get_type() == Texture::Type::SPECULAR) {
			property = "material.texture_specular" + std::to_string(++specularNr);
		}
		shader.set_int(property.c_str(), gsl::narrow<int>(i));
		// bin the texture to that slot
		tex.bind_to_num(i);
		++i;
	}

	draw_mesh();

	for(unsigned int i = 1; const auto &tex : m_textures) {
		Texture::unbind_num(i);
		++i;
	}
	Texture::unbind_num(0);
	Texture::reset();
}

void GlMesh::draw_mesh() const {
	glBindVertexArray(*m_vao);
	if (m_usesEbo) {
		glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	}
	glBindVertexArray(0);
}

void GlMesh::add_texture(const char *path, Texture::Type type, GLenum slot,
						 const Texture::Settings &settings) {
	m_textures.emplace_back(path, settings, slot, type);
}

void GlMesh::add_texture(std::string_view path, Texture::Type type, GLenum slot,
						 const Texture::Settings &settings) {
	m_textures.emplace_back(path, settings, slot, type);
}

void GlMesh::add_texture(const char *path, Texture::Type type,
						 const Texture::Settings &settings) {
	add_texture(path, type, GL_TEXTURE0, settings);
}

void GlMesh::add_texture(std::string_view path, Texture::Type type,
						 const Texture::Settings &settings) {
	add_texture(path, type, GL_TEXTURE0, settings);
}

void GlMesh::finish_setup() {
	if(m_textures.empty()) {
		// add white texture
		m_textures.push_back(Texture::empty_white());
	}
}

const Texture &GlMesh::placeholder_tex() {
	static Texture tex{Texture::empty_black()};
	return tex;
}

GlMesh::Vertex::Vertex(glm::vec3 position, glm::vec3 normal,
					  glm::vec2 texCoords) :
	position{position},
	normal{normal},
	texCoords{texCoords} {}

GlMesh::Vertex::Vertex() : position{}, normal{}, texCoords{} {}
