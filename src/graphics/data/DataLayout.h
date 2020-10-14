//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_DATALAYOUT_H
#define SD3D_DATALAYOUT_H

#include <initializer_list>
#include <vector>

#include <GLFW/glfw3.h>


class DataLayout {
	// TODO somehow make this constexpr enabled
public:
	enum class GlType {
		DOUBLE, FLOAT, INT, SHORT, BYTE, UINT, USHORT, UBYTE
	};

	// This struct doesnt allow for a different order of elements
	struct VertexAttrib {
		GLint size{};
		GlType type{};
		GLboolean normalized{GL_FALSE};
	};

private:
	static constexpr GLenum type_to_opengl(GlType type);
	static constexpr GLsizei type_size(GlType type);

	std::vector<VertexAttrib> m_attribs;

	[[nodiscard]] GLsizei accumulate_size() const;

public:
	DataLayout();
	DataLayout(std::initializer_list<VertexAttrib> attribs);

	// bind all the contained VertexAttribs
	void bind() const;

	DataLayout &operator<<(const VertexAttrib &other);
	void push(const VertexAttrib &other);
};


#endif //SD3D_DATALAYOUT_H
