cmake_minimum_required(VERSION 3.17.3)

project(SD3D)
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif ()

find_path(BFGROUP_LYRA_INCLUDE_DIRS "lyra/arg.hpp")

find_package(imgui CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)

find_package(spdlog CONFIG REQUIRED)

find_package(Assimp CONFIG REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# TODO get from vcpkg
include_directories(${STBI_PATH})

include_directories("${GLAD_PATH}/include")
include_directories("${GLM_PATH}")

add_executable(SD3D
			   ${GLAD_PATH}/src/glad.c
			   src/main.cpp
			   src/graphics/Shader.cpp
			   src/graphics/Texture.cpp
			   src/controls/OrbitCameraController.cpp
			   src/controls/InputHandler.cpp
			   src/controls/GeneralInputHandler.cpp
			   src/graphics/ImGuiHandler.cpp
			   src/graphics/data/GlMesh.cpp
			   src/graphics/data/DataLayout.cpp
			   src/graphics/data/Model.cpp
			   src/io/FileOpenDialog.cpp
			   )

target_include_directories(SD3D PRIVATE ${BFGROUP_LYRA_INCLUDE_DIRS})
target_link_libraries(SD3D PRIVATE
					  glfw
					  ${OPENGL_LIBRARIES}
					  spdlog::spdlog spdlog::spdlog_header_only
					  imgui::imgui
					  assimp::assimp
					  )
