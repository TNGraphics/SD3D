cmake_minimum_required(VERSION 3.17.3)
project(SD3D)
set(CMAKE_CXX_STANDARD 20)

add_library(imgui
			imgui.cpp
			imgui.h
			imgui_draw.cpp
			imgui_internal.h
			imgui_tables.cpp
			imgui_demo.cpp
			imgui_widgets.cpp
			imstb_rectpack.h
			imstb_textedit.h
			imstb_truetype.h
			imgui_impl_opengl3.h
			imgui_impl_opengl3.cpp
			imgui_impl_glfw.h
			imgui_impl_glfw.cpp
			)

target_include_directories(imgui PRIVATE ../glfw/include)
