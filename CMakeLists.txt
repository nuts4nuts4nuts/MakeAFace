cmake_minimum_required(VERSION 2.6)

project(MakeAFace)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glfw3_2/include/GLFW)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/opencv/include)

link_directories(${CMAKE_SOURCE_DIR}/thirdparty/glfw3_2/lib)
link_directories(${CMAKE_SOURCE_DIR}/thirdparty/glew/lib)
link_directories(${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib)

set(SOURCE source)

add_executable(MakeAFace ${SOURCE}/main.cpp
						 ${SOURCE}/GLShader.cpp
						 ${SOURCE}/GLShader.h
						 ${SOURCE}/imconfig.h
						 ${SOURCE}/imgui.cpp
						 ${SOURCE}/imgui.h
						 ${SOURCE}/imgui_demo.cpp
						 ${SOURCE}/imgui_draw.cpp
						 ${SOURCE}/imgui_internal.h
						 ${SOURCE}/imgui_impl_glfw_gl3.h
						 ${SOURCE}/imgui_impl_glfw_gl3.cpp
						 ${SOURCE}/stb_rect_pack.h
						 ${SOURCE}/stb_textedit.h
						 ${SOURCE}/stb_truetype.h)

set(LIBS
	glfw3
	glfw3dll
	opengl32
	opencv_world320
	glew32s)

target_link_libraries(MakeAFace ${LIBS})