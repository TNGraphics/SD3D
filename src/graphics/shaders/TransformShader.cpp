//
// Created by Tobias on 10/21/2020.
//

#include "../memory/gl_memory_helpers.hpp"

#include "TransformShader.hpp"

#include <spdlog/spdlog.h>

TransformShader::TransformShader(std::string_view vertexPath,
								 std::string_view fragmentPath) :
	Shader(vertexPath, fragmentPath) {}

TransformShader::TransformShader(const char *vertSource,
								 const char *fragSource) :
	Shader{vertSource, fragSource} {}

TransformShader::TransformShader(ShaderHandle vertex, ShaderHandle fragment) :
	Shader{vertex, fragment} {}

void TransformShader::apply_transform() const {
	GLenum type = GL_INVALID_ENUM;
	// TODO really bad >:D
	type = m_positionData.normalMatrix
			   ? sd3d::memory::fill_ssbo(m_ssbo, &m_positionData)
			   : sd3d::memory::fill_buffer(m_ssbo, 3 * sizeof(glm::mat4),
										   &m_positionData, GL_DYNAMIC_COPY);
	glBindBufferBase(type, 0, m_ssbo.name());
}

void TransformShader::reset() {
	m_positionData = {};
}

void TransformShader::model(const glm::mat4 &model) {
	m_positionData.model = model;
}

void TransformShader::view(const glm::mat4 &view) {
	m_positionData.view = view;
}

void TransformShader::projection(const glm::mat4 &projection) {
	m_positionData.projection = projection;
}

void TransformShader::normal_mat(const glm::mat3 &normalMat) {
	m_positionData.normalMatrix = normalMat;
}
