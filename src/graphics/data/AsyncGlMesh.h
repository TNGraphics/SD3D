//
// Created by Tobias on 10/16/2021.
//

#ifndef SD3D_ASYNCGLMESH_H
#define SD3D_ASYNCGLMESH_H

#include "GlMesh.h"

#include "detail/assimp_helpers.h"

class AsyncGlMesh : public GlMesh {
private:
	struct TextureLoadInstruction {
		// TODO don't require string_view (allow other string types)
		std::string_view path;
		Texture::Settings settings;
		GLenum slot;
		Texture::Type type;
	};

private:
	using GlMesh::from_ai_mesh;

	AsyncGlMesh(std::vector<sd3d::data::Vertex> data, std::vector<GLuint> indices,
				glm::mat4 transform = glm::mat4{1.0});

	std::vector<sd3d::data::Vertex> m_data;
	std::vector<GLuint> m_indices;
	std::vector<TextureLoadInstruction> m_texturesToLoad;

	void add_texture(const TextureLoadInstruction &);

public:
	static AsyncGlMesh async_from_ai_mesh(aiMesh *, const aiScene *,
										  const std::string &texDir,
										  glm::mat4 transform = glm::mat4{1.0});
	void finalize();
};

#endif // SD3D_ASYNCGLMESH_H
