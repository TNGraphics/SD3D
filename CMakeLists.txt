cmake_minimum_required(VERSION 3.20)

project(SD3D VERSION 0.1.2.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

if (NOT DEFINED PYTHON_PATH)
	find_program(PYTHON_PATH python REQUIRED)
endif()

message("python will be " ${PYTHON_PATH})

if (NOT DEFINED GLSLC_PATH)
	find_program(GLSLC_PATH glslc REQUIRED)
endif()

message("GLSLC will be " ${GLSLC_PATH})

set(SHADERS
	src/graphics/shaders/builtin/src/color.frag
	src/graphics/shaders/builtin/src/color.vert
	src/graphics/shaders/builtin/src/error.frag
	src/graphics/shaders/builtin/src/error.vert
	src/graphics/shaders/builtin/src/lit.frag
	src/graphics/shaders/builtin/src/lit.vert
	)

add_custom_target(
		shader_gen ALL
		COMMAND ${PYTHON_PATH} ./build_scripts/compile_shaders.py ${GLSLC_PATH} ${PROJECT_BINARY_DIR} ${SHADERS}
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
		COMMENT "Generating Shaders"
)

add_subdirectory(external/imgui)

add_subdirectory(external/glfw)

add_subdirectory(external/spdlog)

set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "enable X functionality")
add_subdirectory(external/assimp)

add_subdirectory(external/glad)

add_subdirectory(external/gsl-lite)

add_subdirectory(external/xenium)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# TODO get from vcpkg
include_directories(external/stb)

include_directories(external/glad/include)
include_directories(external/glm)
include_directories(external/imgui)

configure_file(src/include/version.h.in version.h)

add_executable(SD3D
			   src/main.cpp
			   src/graphics/shaders/Shader.cpp
			   src/graphics/data/Texture.cpp
			   src/controls/OrbitCameraController.cpp
			   src/controls/InputHandler.cpp
			   src/controls/GeneralInputHandler.cpp
			   src/graphics/ImGuiHandler.cpp
			   src/graphics/data/GlMesh.cpp
			   src/graphics/data/DataLayout.cpp
			   src/graphics/data/Model.cpp
			   src/io/FileOpenDialog.cpp
			   src/graphics/GlContext.cpp
			   src/external/imgui-filebrowser/imfilebrowser.cpp
			   src/graphics/memory/gl_memory.cpp
			   src/graphics/shaders/TransformShader.cpp
			   src/graphics/shaders/LitShader.cpp
			   src/graphics/shaders/ColorShader.cpp
			   src/graphics/data/detail/AssimpNode.cpp
			   src/graphics/data/AsyncModel.cpp
			   src/graphics/data/AsyncGlMesh.cpp
			   src/graphics/data/detail/assimp_helpers.cpp
			   src/graphics/data/detail/AsyncAssimpNode.cpp
			   src/objects/Transform.cpp
#			   src/objects/Object.cpp
#			   src/scene/SceneTree.cpp
			   )

add_dependencies(SD3D shader_gen)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(SD3D PUBLIC -DDEBUG=1)
endif()

if(DEFINED MEMORY_DEBUG AND MEMORY_DEBUG)
	target_compile_definitions(SD3D PUBLIC -DMEM_DEBUG)
endif()

if (MSVC)
	target_compile_options(SD3D PRIVATE /W4 /WX)
else ()
	target_compile_options(SD3D PRIVATE -std=c++20)
#	target_compile_options(SD3D PRIVATE -Wall -Wextra -Wpedantic)
#	target_compile_options(SD3D PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

target_include_directories(SD3D PRIVATE
						   ${PROJECT_BINARY_DIR}
						   external/glad/include
						   external/imgui/imgui
						   external/xenium
		                   external/gsl-lite/include
						   external/lyra/include
						   )

target_link_libraries(SD3D PRIVATE
					  glfw
					  ${OPENGL_LIBRARIES}
					  spdlog::spdlog spdlog::spdlog_header_only
					  imgui
					  assimp::assimp
					  gsl::gsl-lite-v1
					  glad
					  )
