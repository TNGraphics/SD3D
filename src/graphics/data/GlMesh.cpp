//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "DataLayout.h"

#include "GlMesh.h"


const DataLayout &GlMesh::vertex_layout() {
	static DataLayout d{{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{3, DataLayout::GlType::FLOAT, GL_FALSE},
						{2, DataLayout::GlType::FLOAT, GL_FALSE}};
	return d;
}

GlMesh GlMesh::from_data(const DataLayout &dataLayout, const float *data,
						 GLuint amount) {
	GLuint vao, vbo;
	// Generate a Vertex Array Object (for now without data)
	glGenVertexArrays(1, &vao);
	// Generate a Vertex Buffer object (also without data)
	glGenBuffers(1, &vbo);
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(vao);

	// Bind the VBO to fill it with data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Fill the VBO with data
	glBufferData(GL_ARRAY_BUFFER, amount, data, GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	dataLayout.bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// TODO unbind dataLayout as well somehow

	return GlMesh(vao, amount, vbo, 0, false);
}

GlMesh GlMesh::from_data(const std::vector<Vertex> &data,
						 const std::vector<GLuint> &indices) {
	GLuint vao, vbo, ebo;
	// Generate a Vertex Array Object (for now without data)
	glGenVertexArrays(1, &vao);
	// Generate a Vertex Buffer object (also without data)
	glGenBuffers(1, &vbo);
	// Generate an Element Buffer Object
	glGenBuffers(1, &ebo);
	// Bind the VAO so upcoming changes are saved here
	glBindVertexArray(vao);

	// Bind the VBO to fill it with data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Fill the VBO with data
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex),
				 data.data(), GL_STATIC_DRAW);

	// Bind and fill EBO with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 indices.data(), GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	vertex_layout().bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// TODO unbind dataLayout as well somehow

	return GlMesh(vao, static_cast<GLuint>(indices.size()), vbo, ebo, true);
}

GlMesh::GlMesh(GLuint vao, GLuint drawCount, GLuint vbo, GLuint ebo,
			   bool useEbo) :
	m_vao{vao},
	m_drawCount{drawCount},
	m_vbo{vbo},
	m_ebo{ebo},
	m_usesEbo{useEbo},
	m_initialized{true} {}

GlMesh::GlMesh(GlMesh &&other) noexcept :
	m_vao{other.m_vao},
	m_drawCount{other.m_drawCount},
	m_vbo{other.m_vbo},
	m_ebo{other.m_ebo},
	m_usesEbo{other.m_usesEbo},
	m_initialized{other.m_initialized} {
	other.m_vao = 0;
	other.m_drawCount = 0;
	other.m_vbo = 0;
	other.m_ebo = 0;
	other.m_usesEbo = false;
	other.m_initialized = false;
}

GlMesh &GlMesh::operator=(GlMesh &&other) noexcept {
	m_vao = other.m_vao;
	m_drawCount = other.m_drawCount;
	m_vbo = other.m_vbo;
	m_ebo = other.m_ebo;
	m_usesEbo = other.m_usesEbo;
	m_initialized = other.m_initialized;

	other.m_vao = 0;
	other.m_drawCount = 0;
	other.m_vbo = 0;
	other.m_ebo = 0;
	other.m_usesEbo = false;
	other.m_initialized = false;
	return *this;
}

void GlMesh::draw() const {
	if (!m_initialized) return;
	glBindVertexArray(m_vao);
	if (m_usesEbo) {
		glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	}
	glBindVertexArray(0);
}

void GlMesh::release_data() {
	if (!m_initialized) return;
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	if (m_usesEbo) {
		glDeleteBuffers(1, &m_ebo);
		m_usesEbo = false;
	}
	m_drawCount = 0;
	m_initialized = false;
}

GlMesh::Vertex::Vertex(glm::vec3 position, glm::vec3 normal,
					  glm::vec2 texCoords) :
	position{position},
	normal{normal},
	texCoords{texCoords} {}

GlMesh::Vertex::Vertex() : position{}, normal{}, texCoords{} {}
