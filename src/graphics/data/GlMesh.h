//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_GLMESH_H
#define SD3D_GLMESH_H

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "../memory/gl_memory.h"

class DataLayout;

class GlMesh {
public:
	struct Vertex {
		[[maybe_unused]] glm::vec3 position{};
		[[maybe_unused]] glm::vec3 normal{};
		[[maybe_unused]] glm::vec2 texCoords{};

		Vertex();
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	};

	static const DataLayout &vertex_layout();

private:
	// Vertex Array Object
	// - This object holds info about data layout, the VBO the data is coming
	// from
	//   and the linked EBO (if applicable)
	sd3d::memory::shared_vao_t m_vao;
	GLuint m_drawCount;

	// TODO save references to textures here, because they are per model (i
	// think)

	// The VBO is implicitly saved in the VAO
	// We still need it to delete it later
	sd3d::memory::shared_vbo_t m_vbo;
	// Same with EBO
	sd3d::memory::shared_ebo_t m_ebo;

	// Notify if an EBO was created and bound to the VAO
	// If yes, glDrawElements has to be used.
	bool m_usesEbo;

	bool m_initialized{false};

	GlMesh(sd3d::memory::shared_vao_t vao, GLuint drawCount,
		   sd3d::memory::shared_vbo_t vbo, sd3d::memory::shared_ebo_t ebo,
		   bool useEbo);

	void draw_mesh() const;

public:
	GlMesh(GlMesh &&) noexcept;
	GlMesh &operator=(GlMesh &&) noexcept;
	GlMesh() = delete;
	GlMesh(const GlMesh &) = default;
	GlMesh &operator=(const GlMesh &);

	void draw() const;

	// for now only float
	static GlMesh from_data(const DataLayout &dataLayout, const float *data,
							GLuint amount);

	static GlMesh from_data(const std::vector<Vertex> &data,
							const std::vector<GLuint> &indices);
};

#endif // SD3D_GLMESH_H
