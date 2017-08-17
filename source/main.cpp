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
	GLuint purpleSquareShader = 0;
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
		purpleSquareShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\_purple_square.glsl");
	}

	// Load the shader
	GLuint textureCopyShader = 0;
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
		textureCopyShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\texture_copy.glsl");
	}

	// Load the shader
	GLuint makeAFaceMainShader = 0;
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
		makeAFaceMainShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_main.glsl");
	}

	// Load the shader
	GLuint textureDrawShader = 0;
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
		textureDrawShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\texture_draw.glsl");
	}

	//turn off vsync
	glfwSwapInterval(0);

	//Initialize frame buffer
	//Code copied from http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
	GLuint referenceFramebuffer = 0;
	GLuint referenceFBTexture = 0;
	{
		glGenFramebuffers(1, &referenceFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);

		//Give the frame buffer a texture the same size as the window
		glGenTextures(1, &referenceFBTexture);
		glBindTexture(GL_TEXTURE_2D, referenceFBTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Attach
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, referenceFBTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint cameraFramebuffer = 0;
	GLuint cameraFBTexture = 0;
	{
		glGenFramebuffers(1, &cameraFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, cameraFramebuffer);

		//Give the frame buffer a texture the same size as the window
		glGenTextures(1, &cameraFBTexture);
		glBindTexture(GL_TEXTURE_2D, cameraFBTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Attach
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, cameraFBTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint currentFramebuffer = 0;
	GLuint currentFBTexture = 0;
	{
		glGenFramebuffers(1, &currentFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);

		//Give the frame buffer a texture the same size as the window
		glGenTextures(1, &currentFBTexture);
		glBindTexture(GL_TEXTURE_2D, currentFBTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Attach
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, currentFBTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

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

		//Copy camera????!?!? to camera frame buffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, cameraFramebuffer);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(1, drawBuffers);

			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(purpleSquareShader);

			int uniformWindowSize = glGetUniformLocation(purpleSquareShader, "iResolution");
			glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

			int uniformTime = glGetUniformLocation(purpleSquareShader, "iGlobalTime");
			glUniform1f(uniformTime, ((float)currentTime));

			drawQuad();
		}

		//Test new image in current buffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(1, drawBuffers);

			glClear(GL_COLOR_BUFFER_BIT);

			int uniformWindowSize = glGetUniformLocation(makeAFaceMainShader, "iResolution");
			int uniformTime = glGetUniformLocation(makeAFaceMainShader, "iTime");
			int referenceTexture = glGetUniformLocation(makeAFaceMainShader, "previousTexture");
			int trueTexture = glGetUniformLocation(makeAFaceMainShader, "trueTexture");
			glUseProgram(makeAFaceMainShader);
			glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);
			glUniform1f(uniformTime, ((float)currentTime));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, referenceFBTexture);
			glUniform1i(referenceTexture, 0);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, cameraFBTexture);
			glUniform1i(trueTexture, 1);

			drawQuad();
		}

		//Copy current texture to previous texture
		{
			glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			
			glClear(GL_COLOR_BUFFER_BIT);

			int textureToRender = glGetUniformLocation(textureCopyShader, "renderedTexture");
			int uniformWindowSize = glGetUniformLocation(textureCopyShader, "iResolution");
			glUseProgram(textureCopyShader);
			glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, currentFBTexture);
			glUniform1i(textureToRender, 2);

			drawQuad();
		}

		//Display current buffer to screen
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(textureDrawShader);

			int uniformWindowSize = glGetUniformLocation(textureDrawShader, "iResolution");
			glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

			int textureToRender = glGetUniformLocation(textureDrawShader, "renderedTexture");
			glUniform1i(textureToRender, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, currentFBTexture);

			drawQuad();
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}