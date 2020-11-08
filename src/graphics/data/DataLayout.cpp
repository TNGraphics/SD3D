//
// Created by Tobias on 10/13/2020.
//

#include <glad/glad.h>

#include "DataLayout.h"

DataLayout::DataLayout() : m_attribs{} {}

DataLayout::DataLayout(std::initializer_list<VertexAttrib> attribs) :
	m_attribs{attribs} {}

void DataLayout::bind() const {
	auto stride = accumulate_size();
	size_t pos{};
	for (unsigned int index{}; const auto &attrib : m_attribs) {
		if (attrib.type != GlType::FILLER) {
			glVertexAttribPointer(index, attrib.size,
								  type_to_opengl(attrib.type),
								  attrib.normalized, stride, (void *)pos);
			glEnableVertexAttribArray(index);
		}
		pos += attrib.size * type_size(attrib.type);
		++index;
	}
}

DataLayout &DataLayout::operator<<(const DataLayout::VertexAttrib &other) {
	m_attribs.push_back(other);
	return *this;
}

void DataLayout::push(const DataLayout::VertexAttrib &other) {
	m_attribs.push_back(other);
}

GLsizei DataLayout::accumulate_size() const {
	GLsizei size{};
	for (const auto &val : m_attribs) {
		size += val.size * type_size(val.type);
	}
	return size;
}

constexpr GLenum DataLayout::type_to_opengl(DataLayout::GlType type) {
	switch (type) {
	case GlType::DOUBLE:
		return GL_DOUBLE;
	case GlType::FLOAT:
		return GL_FLOAT;
	case GlType::INT:
		return GL_INT;
	case GlType::SHORT:
		return GL_SHORT;
	case GlType::BYTE:
		return GL_BYTE;
	case GlType::UINT:
		return GL_UNSIGNED_INT;
	case GlType::USHORT:
		return GL_UNSIGNED_SHORT;
	case GlType::UBYTE:
		return GL_UNSIGNED_BYTE;
	case GlType::FILLER:
		return GL_INVALID_ENUM;
	}
	return GL_INVALID_ENUM;
}

constexpr GLsizei DataLayout::type_size(DataLayout::GlType type) {
	switch (type) {
		// TODO maybe use GL types, like GLuint, etc.
	case GlType::DOUBLE:
		return sizeof(double);
	case GlType::FLOAT:
		return sizeof(float);
	case GlType::INT:
		return sizeof(int);
	case GlType::SHORT:
		return sizeof(short);
	case GlType::BYTE:
		return sizeof(char);
	case GlType::UINT:
		return sizeof(unsigned int);
	case GlType::USHORT:
		return sizeof(unsigned short);
	case GlType::UBYTE:
		return sizeof(unsigned char);
	case GlType::FILLER:
		return 0;
	}
	return 0;
}
