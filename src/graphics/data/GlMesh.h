//
// Created by Tobias on 10/13/2020.
//

#ifndef SD3D_GLMESH_H
#define SD3D_GLMESH_H

#include <vector>

#include <GLFW/glfw3.h>

#pragma warning(push, 0)

#include <glm/glm.hpp>

#pragma warning(pop)

#include "../memory/gl_memory.h"

#include "Texture.h"

class DataLayout;
class LitShader;

struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

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

	// The VBO is implicitly saved in the VAO
	// We still need it to delete it later
	sd3d::memory::shared_vbo_t m_vbo;
	// Same with EBO
	sd3d::memory::shared_ebo_t m_ebo;

	// Notify if an EBO was created and bound to the VAO
	// If yes, glDrawElements has to be used.
	bool m_usesEbo;

	bool m_initialized{false};

	std::vector<Texture> m_textures{};

	glm::mat4 m_modelMatrix;
	glm::mat3 m_normalMatrix;

	static const Texture &placeholder_tex();

	void draw_mesh() const;

	void process_material(aiMaterial *mat, const std::string &texDir);
	void process_material_textures_of_type(aiMaterial *, aiTextureType,
										   const std::string &texDir);

public:
	GlMesh(GlMesh &&) noexcept;
	GlMesh &operator=(GlMesh &&) noexcept;
	GlMesh() = delete;
	GlMesh(const GlMesh &) = default;
	GlMesh &operator=(const GlMesh &);

	void draw() const;
	void draw(LitShader &) const;

	GlMesh(const DataLayout &dataLayout, const float *data, GLuint amount,
		   glm::mat4 transform = glm::mat4{1.0});

	GlMesh(const std::vector<Vertex> &data, const std::vector<GLuint> &indices,
		   glm::mat4 transform = glm::mat4{1.0});

	static GlMesh from_ai_mesh(aiMesh *, const aiScene *,
							   const std::string &texDir,
							   glm::mat4 transform = glm::mat4{1.0});

	void add_texture(const char *path, Texture::Type type, GLenum slot,
					 const Texture::Settings &settings = Texture::Settings{});
	void add_texture(std::string_view path, Texture::Type type, GLenum slot,
					 const Texture::Settings &settings = Texture::Settings{});
	void add_texture(const char *path, Texture::Type type,
					 const Texture::Settings &settings = Texture::Settings{});
	void add_texture(std::string_view path, Texture::Type type,
					 const Texture::Settings &settings = Texture::Settings{});

	void set_transform(glm::mat4 transform);

	void finish_setup();
};

#endif // SD3D_GLMESH_H
