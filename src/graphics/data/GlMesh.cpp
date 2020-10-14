//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>

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
	// TODO different modes apart from GL_STATIC_DRAW
	// Fill the VBO with data
	glBufferData(GL_ARRAY_BUFFER, amount, data, GL_STATIC_DRAW);

	// Bind the DataLayout (basically multiple glVertexAttribPointers enabled)
	dataLayout.bind();

	// properly unbind all buffers, arrays, etc. to prevent collisions
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// TODO unbind dataLayout as well somehow

	return GlMesh(vao, amount);
}

GlMesh::GlMesh(GLuint vao, GLuint vertexCount) : m_vao{vao}, m_vertexCount{vertexCount} {}

void GlMesh::draw() const {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
	glBindVertexArray(0);
}
