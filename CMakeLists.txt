cmake_minimum_required(VERSION 3.17.3)

project(SD3D)
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif ()

include_directories(${GLFW_PATH}/include)
link_directories(${GLFW_PATH}/lib)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

include_directories("${GLAD_PATH}/include")

add_executable(SD3D
			   src/main.cpp
			   ${GLAD_PATH}/src/glad.c)
target_link_libraries(SD3D glfw3.lib ${OPENGL_LIBRARIES})
