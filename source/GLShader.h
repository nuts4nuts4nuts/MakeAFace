/*
GLShader files brought in from http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
*/

#ifndef GLSHADER_H
#define GLSHADER_H

#define GLEW_STATIC
#include "glew.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


GLuint LoadShader(const char *vertex_path, const char *fragment_path);

#endif