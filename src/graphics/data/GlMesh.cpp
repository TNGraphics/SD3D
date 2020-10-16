//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "DataLayout.h"

#include "GlMesh.h"

GlMesh GlMesh::from_data(const DataLayout &dataLayout, const float *data, GLuint amount) {
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

	return GlMesh(vao, amount, false);
}

GlMesh GlMesh::from_data(const std::vector<Model::Vertex> &data, const std::vector<GLuint> &indices) {
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
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Model::Vertex), data.data(), GL_STATIC_DRAW);

	// Bind and fill EBO with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	Model::vertex_layout().bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// TODO unbind dataLayout as well somehow

	return GlMesh(vao, static_cast<GLuint>(indices.size()), true);
}

GlMesh::GlMesh(GLuint vao, GLuint drawCount, bool useEbo) : m_vao{vao}, m_drawCount{drawCount}, m_usesEbo{useEbo} {}

void GlMesh::draw() const {
	glBindVertexArray(m_vao);
	if (m_usesEbo) {
		glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	}
	glBindVertexArray(0);
}
