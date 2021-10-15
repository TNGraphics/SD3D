cmake_minimum_required(VERSION 3.17.3)

project(SD3D)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif ()

find_path(BFGROUP_LYRA_INCLUDE_DIRS "lyra/arg.hpp")

add_subdirectory(external/imgui)

add_subdirectory(external/glfw)

find_package(spdlog CONFIG REQUIRED)

find_package(Assimp CONFIG REQUIRED)

find_package(gsl-lite CONFIG REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# TODO get from vcpkg
include_directories(${STBI_PATH})

include_directories(${GLAD_PATH}/include)
include_directories(${GLM_PATH})
include_directories(external/imgui)

add_executable(SD3D
			   ${GLAD_PATH}/src/glad.c
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
			   )

if (CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(SD3D PUBLIC -DDEBUG=1)
endif()

if(DEFINED MEMORY_DEBUG AND MEMORY_DEBUG)
	target_compile_definitions(SD3D PUBLIC -DMEM_DEBUG)
endif()

target_include_directories(SD3D PRIVATE ${BFGROUP_LYRA_INCLUDE_DIRS} external/imgui/imgui)
target_link_libraries(SD3D PRIVATE
					  glfw
					  ${OPENGL_LIBRARIES}
					  spdlog::spdlog spdlog::spdlog_header_only
					  imgui
					  assimp::assimp
					  gsl::gsl-lite-v1
					  )
