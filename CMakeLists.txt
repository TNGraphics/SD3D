cmake_minimum_required(VERSION 3.17.3)

project(SD3D)
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif ()

find_path(BFGROUP_LYRA_INCLUDE_DIRS "lyra/arg.hpp")

include_directories(${GLFW_PATH}/include)
link_directories(${GLFW_PATH}/lib)

find_package(spdlog CONFIG REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

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
			   )

target_include_directories(SD3D PRIVATE ${BFGROUP_LYRA_INCLUDE_DIRS})
target_link_libraries(SD3D PRIVATE glfw3.lib ${OPENGL_LIBRARIES} spdlog::spdlog spdlog::spdlog_header_only)
