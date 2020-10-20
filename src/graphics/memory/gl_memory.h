//
// Created by Tobias on 10/20/2020.
//

#ifndef SD3D_GL_MEMORY_H
#define SD3D_GL_MEMORY_H

#include <memory>

#include <GLFW/glfw3.h>

namespace sd3d::memory {

using shared_vbo_t = std::shared_ptr<GLuint>;

using shared_vao_t = std::shared_ptr<GLuint>;

using shared_ebo_t = std::shared_ptr<GLuint>;

using shared_tex_t = std::shared_ptr<GLuint>;
using weak_tex_t = std::weak_ptr<GLuint>;

using shared_prog_t = std::shared_ptr<GLuint>;

using shared_fbo_t = std::shared_ptr<GLuint>;

using shared_rbo_t = std::shared_ptr<GLuint>;

[[maybe_unused]] shared_vbo_t create_vbo();

[[maybe_unused]] shared_vao_t create_vao();

[[maybe_unused]] shared_ebo_t create_ebo();

[[maybe_unused]] shared_tex_t create_tex();

[[maybe_unused]] shared_prog_t create_prog();

[[maybe_unused]] shared_fbo_t create_fbo();

[[maybe_unused]] shared_rbo_t create_rbo();

} // namespace sd3d::memory

#endif // SD3D_GL_MEMORY_H
