//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_MESH_H
#define SD3D_MESH_H

#include <GLFW/glfw3.h>
#include "DataLayout.h"

class Mesh {
	// TODO data structure for data layout
private:
	// Vertex Array Object
	// - This object holds info about data layout, the VBO the data is coming from
	//   and the linked EBO (if applicable)
	GLuint m_vao;
	GLuint m_vertexCount;

	// The VBO is implicitly saved in the VAO

	// TODO support EBO later
//	// Element Buffer Object
//	// - This object doesn't have to be bound
//	// - If it is bound, it provides a way to reuse vertices multiple times by
//	//   specifying the indices into the VBO that are used
//	GLuint m_ebo;

	explicit Mesh(GLuint vao, GLuint vertexCount);

public:
	void draw() const;

	// for now only float
	static Mesh from_data(const DataLayout &dataLayout, const float *data, GLuint amount);

};


#endif //SD3D_MESH_H
