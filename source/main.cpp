#include <iostream>
#include <fstream>
#include <thread>

#define GLEW_STATIC
#include <glew.h>

#include <glfw3.h>

#include "opencv2/highgui/highgui.hpp"
#include "GLShader.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

cv::Mat captureFrame;
bool keepCapturing = true;

bool showImGuiWindow = true;

void captureFrames()
{
	cv::VideoCapture captureDevice(0);
	if (!captureDevice.isOpened())
	{
		std::cout << "Cannot open the camera" << std::endl;
	}

	double captureWidth = captureDevice.get(CV_CAP_PROP_FRAME_WIDTH);
	double captureHeight = captureDevice.get(CV_CAP_PROP_FRAME_HEIGHT);

	std::printf("Frame size: %f x %f\n", captureWidth, captureHeight);

	while (keepCapturing)
	{
		captureDevice.read(captureFrame);
	}
}

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A  && action == GLFW_PRESS)
	{
		showImGuiWindow = !showImGuiWindow;
	}
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

	ImGui_ImplGlfwGL3_Init(window, true);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1, 1, 1, 0);

	// Load the shader
	GLuint textureCopyShader = 0;
	{
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		//glShaderSource(vertexShader, 1, /*SHADER*/, nullptr);
		//glCompileShader(vertexShader);

		//GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		//glShaderSource(fragShader, 1, /*SHAcameraTextureDER*/, nullptr);
		//glCompileShader(fragShader);

		//GLuint shaderProgram = glCreateProgram();
		//glAttachShader(shaderProgram, fragShader);
		//glAttachShader(shaderProgram, vertexShader);
		//glLinkProgram(shaderProgram);

		//shaderProgram = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_fs.glsl");
		textureCopyShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\texture_copy.glsl");
	}

	// Load the shader
	GLuint textureDrawShader = 0;
	{
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		//glShaderSource(vertexShader, 1, /*SHADER*/, nullptr);
		//glCompileShader(vertexShader);

		//GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		//glShaderSource(fragShader, 1, /*SHAcameraTextureDER*/, nullptr);
		//glCompileShader(fragShader);

		//GLuint shaderProgram = glCreateProgram();
		//glAttachShader(shaderProgram, fragShader);
		//glAttachShader(shaderProgram, vertexShader);
		//glLinkProgram(shaderProgram);

		//shaderProgram = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_fs.glsl");
		textureDrawShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\texture_draw.glsl");
	}

	// Load the shader
	GLuint makeAFaceDraw = 0;
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
		makeAFaceDraw = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_draw.glsl");
	}

	//turn off vsync
	glfwSwapInterval(0);

	//Initialize frame buffer
	//Code copied from http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
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
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, currentFBTexture, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}
	}

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

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}
	}

	GLuint finalFramebuffer = 0;
	GLuint finalFBTexture = 0;
	{
		glGenFramebuffers(1, &finalFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);

		//Give the frame buffer a texture the same size as the window
		glGenTextures(1, &finalFBTexture);
		glBindTexture(GL_TEXTURE_2D, finalFBTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Attach
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, finalFBTexture, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}
	}

	GLuint cameraTexture = 0;
	{
		//Give the frame buffer a texture the same size as the window
		glGenTextures(1, &cameraTexture);
		glBindTexture(GL_TEXTURE_2D, cameraTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	GLuint atomicCounterBuffer = 0;
	{
		glGenBuffers(1, &atomicCounterBuffer);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 2, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	std::thread captureThread(captureFrames);

	//draw loop
	double frameRateMarkerDraw = glfwGetTime();
	double frameRateMarkerUI = glfwGetTime();

	//ImGui variables
	int targetFrameRate = 500;
	bool useTriangles = true;
	bool everyPixelSameColor = true;
	bool sourceColors = false;
	bool transparency = false;
	bool allNew = false;

	glfwSetKeyCallback(window, keyCallback);

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();

		glfwPollEvents();

		if (currentTime - frameRateMarkerUI >= (1.0 / std::fmax((double)targetFrameRate, 60.0)))
		{
			frameRateMarkerUI = currentTime;

			//ImGui stuff
			if (showImGuiWindow)
			{
				ImGui_ImplGlfwGL3_NewFrame();

				ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
				ImGui::Begin("Settings - Press A to toggle");
				ImGui::Checkbox("Triangles", &useTriangles);
				ImGui::Checkbox("Same Color", &everyPixelSameColor);
				ImGui::Checkbox("Source Color", &sourceColors);
				ImGui::Checkbox("Transparency", &transparency);
				ImGui::Checkbox("All New", &allNew);
				ImGui::SliderInt("Target FPS", &targetFrameRate, 1, 1500);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				if (ImGui::SmallButton("Reset"))
				{
					glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);
					glClear(GL_COLOR_BUFFER_BIT);
				}

				ImGui::End();
			}

			if (currentTime - frameRateMarkerDraw >= (1.0 / (double)targetFrameRate))
			{
				frameRateMarkerDraw = currentTime;

				//Copy camera to a texture
				{
					glBindTexture(GL_TEXTURE_2D, cameraTexture);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, captureFrame.size().width, captureFrame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, captureFrame.ptr());
				}

				//Draw test shape
				{
					//Reset atomic counters
					glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
					GLuint a[2] = { 0, 0 };
					glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 2, a);
					glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

					glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
					glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterBuffer);

					glClear(GL_COLOR_BUFFER_BIT);

					//Static uniforms
					int uniformWindowSize = glGetUniformLocation(makeAFaceDraw, "iResolution");
					int uniformTime = glGetUniformLocation(makeAFaceDraw, "iTime");
					int trueTexture = glGetUniformLocation(makeAFaceDraw, "trueTexture");
					int referenceTexture = glGetUniformLocation(makeAFaceDraw, "referenceTexture");

					//Settings
					int triangleSetting = glGetUniformLocation(makeAFaceDraw, "useTriangles");
					int pixelColorSetting = glGetUniformLocation(makeAFaceDraw, "everyPixelSameColor");
					int sourceColorSetting = glGetUniformLocation(makeAFaceDraw, "sourceColors");
					int transparencySetting = glGetUniformLocation(makeAFaceDraw, "transparency");
					int allNewSetting = glGetUniformLocation(makeAFaceDraw, "allNew");

					glUseProgram(makeAFaceDraw);
					glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);
					glUniform1f(uniformTime, ((float)currentTime));

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, cameraTexture);
					glUniform1i(trueTexture, 0);

					glActiveTexture(GL_TEXTURE0 + 1);
					glBindTexture(GL_TEXTURE_2D, referenceFBTexture);
					glUniform1i(referenceTexture, 1);

					glUniform1i(triangleSetting, useTriangles);
					glUniform1i(pixelColorSetting, everyPixelSameColor);
					glUniform1i(sourceColorSetting, sourceColors);
					glUniform1i(transparencySetting, transparency);
					glUniform1i(allNewSetting, allNew);

					drawQuad();
				}

				//Copy final image
				{
					//Get atomic counter data
					GLuint atomicCounterValues[2];
					glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
					glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 2, atomicCounterValues);
					glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

					glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);

					glClear(GL_COLOR_BUFFER_BIT);

					glUseProgram(textureCopyShader);

					int uniformWindowSize = glGetUniformLocation(textureCopyShader, "iResolution");
					glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

					int renderedTexture = glGetUniformLocation(textureCopyShader, "renderedTexture");
					glActiveTexture(GL_TEXTURE0 + 2);

					if (!allNew || atomicCounterValues[0] * 1.8 > atomicCounterValues[1])
					{
						glBindTexture(GL_TEXTURE_2D, currentFBTexture);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, referenceFBTexture);
					}

					glUniform1i(renderedTexture, 2);

					drawQuad();
				}

				//Copy final image
				{
					glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);

					glClear(GL_COLOR_BUFFER_BIT);

					glUseProgram(textureCopyShader);

					int uniformWindowSize = glGetUniformLocation(textureCopyShader, "iResolution");
					glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

					int renderedTexture = glGetUniformLocation(textureCopyShader, "renderedTexture");
					glActiveTexture(GL_TEXTURE0 + 2);
					glBindTexture(GL_TEXTURE_2D, finalFBTexture);
					glUniform1i(renderedTexture, 2);

					drawQuad();
				}

				//Draw final image
				{
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					glClear(GL_COLOR_BUFFER_BIT);

					glUseProgram(textureDrawShader);

					int uniformWindowSize = glGetUniformLocation(textureDrawShader, "iResolution");
					glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);

					int renderedTexture = glGetUniformLocation(textureDrawShader, "renderedTexture");
					glActiveTexture(GL_TEXTURE0 + 2);
					glBindTexture(GL_TEXTURE_2D, finalFBTexture);
					glUniform1i(renderedTexture, 2);

					drawQuad();
				}

				if (showImGuiWindow)
				{
					ImGui::Render();
				}

				glfwSwapBuffers(window);
			}
		}
	}

	keepCapturing = false;
	captureThread.join();

	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	return 0;
}