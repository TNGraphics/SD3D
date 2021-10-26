//
// Created by Tobias on 10/21/2020.
//

#include "TransformShader.hpp"

TransformShader::TransformShader(std::string_view vertexPath,
								 std::string_view fragmentPath) :
	Shader(vertexPath, fragmentPath) {
	init_locations();
}

TransformShader::TransformShader(const char *vertSource,
								 const char *fragSource) :
	Shader{vertSource, fragSource} {
	init_locations();
}

void TransformShader::init_locations() {
	m_modelLoc = get_uniform_loc("model");
	m_viewLoc = get_uniform_loc("view");
	m_projLoc = get_uniform_loc("projection");
	m_normMatLoc = get_uniform_loc("normalMatrix");
}

void TransformShader::model(const glm::mat4 &modelMat) const {
	set(m_modelLoc, modelMat);
}

void TransformShader::view(const glm::mat4 &viewMat) const {
	set(m_viewLoc, viewMat);
}

void TransformShader::projection(const glm::mat4 &projMat) const {
	set(m_projLoc, projMat);
}

void TransformShader::normal_mat(const glm::mat3 &normMat) const {
	set(m_normMatLoc, normMat);
}
