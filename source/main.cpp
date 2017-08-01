#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <glew.h>

#include <glfw3.h>
#include <cv.h>

#include "GLShader.h"

void drawQuad()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Initialize glew
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::printf("Renderer: %s\n", renderer);
	std::printf("Version: %s\n", version);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Load the shader
	GLuint shaderProgram = 0;
	{
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		//glShaderSource(vertexShader, 1, /*SHADER*/, nullptr);
		//glCompileShader(vertexShader);

		//GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		//glShaderSource(fragShader, 1, /*SHADER*/, nullptr);
		//glCompileShader(fragShader);

		//GLuint shaderProgram = glCreateProgram();
		//glAttachShader(shaderProgram, fragShader);
		//glAttachShader(shaderProgram, vertexShader);
		//glLinkProgram(shaderProgram);

		//shaderProgram = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_fs.glsl");
		shaderProgram = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\test.glsl");
	}

	//Initialize frame buffer
	//GLuint myFramebuffer = 0;
	//{
	//	glGenFramebuffers(1, &myFramebuffer);
	//	glBindFramebuffer(GL_FRAMEBUFFER, myFramebuffer);

	//	//Give the frame buffer a texture the same size as the window
	//	GLuint texture = 0;
	//	glGenTextures(1, &texture);
	//	glBindTexture(GL_TEXTURE_2D, texture);

	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//	//Attach 
	//}

	//turn off vsync
	glfwSwapInterval(0);

	//draw loop
	double previousTime = glfwGetTime();
	int frames = 0;
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		frames++;

		if (currentTime - previousTime >= 1.0)
		{
			std::printf("%f ms / frame\n", 1000.0 / double(frames));
			frames = 0;
			previousTime = currentTime;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		int uniformWindowSize = glGetUniformLocation(shaderProgram, "iResolution");
		int uniformTime = glGetUniformLocation(shaderProgram, "iGlobalTime");
		glUseProgram(shaderProgram);
		glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);
		glUniform1f(uniformTime, ((float)currentTime));

		drawQuad();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}