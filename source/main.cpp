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

	// Load the shader
	GLuint textureCopyShader = 0;
	{
		textureCopyShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\texture_copy.glsl");
	}
	// Load the shader
	GLuint makeAFaceMainShader = 0;
	{
		makeAFaceMainShader = LoadShader("shaders\\make_a_face_vs.glsl", "shaders\\make_a_face_main.glsl");
	}
	// Load the shader
	GLuint textureDrawShader = 0;
	{
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

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return -1;
		}
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
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, currentFBTexture, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT1 };
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

	std::thread captureThread(captureFrames);

	//draw loop
	double frameRateMarker = glfwGetTime();

	//ImGui variables
	int targetFrameRate = 500;
	bool useTriangles = true;
	bool everyPixelSameColor = true;
	bool sourceColors = false;
	char picName[256] = "SavedImage.jpg";

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		if (currentTime - frameRateMarker >= (1.0 / (double)targetFrameRate))
		{
			frameRateMarker = currentTime;

			glfwPollEvents();

			ImGuiIO io = ImGui::GetIO();
			if (io.KeyAlt)
			{
				if (ImGui::IsKeyPressed(65))
				{
					showImGuiWindow = !showImGuiWindow;
				}
			}

			//ImGui stuff
			if(showImGuiWindow)
			{
				ImGui_ImplGlfwGL3_NewFrame();

				ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
				ImGui::Begin("Settings - Press Ctril + A to toggle");
				ImGui::Checkbox("Triangles", &useTriangles);
				ImGui::Checkbox("Same Color", &everyPixelSameColor);
				ImGui::Checkbox("Source Color", &sourceColors);
				ImGui::SliderInt("Target FPS", &targetFrameRate, 5, 1500);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::Text("----------------------");

				ImGui::InputText("Pic name", picName, 256);
				if (ImGui::SmallButton("Snap"))
				{
					cv::Mat saveImage(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
					glReadPixels(0, 0, saveImage.cols, saveImage.rows, GL_BGR, GL_UNSIGNED_BYTE, saveImage.data);
					cv::flip(saveImage, saveImage, 0);
					cv::imshow(picName, saveImage);
					cv::imwrite(picName, saveImage);
				}

				if (ImGui::SmallButton("Shoot"))
				{
				}

				if (ImGui::SmallButton("Reset"))
				{
					glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);
					glClear(GL_COLOR_BUFFER_BIT);
				}

				ImGui::ShowTestWindow();
				ImGui::End();
			}

			//Copy camera to a texture
			{
				glBindTexture(GL_TEXTURE_2D, cameraTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, captureFrame.size().width, captureFrame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, captureFrame.ptr());
			}

			//Test new image in current buffer
			{
				glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);

				glClear(GL_COLOR_BUFFER_BIT);

				//Static uniforms
				int uniformWindowSize = glGetUniformLocation(makeAFaceMainShader, "iResolution");
				int uniformTime = glGetUniformLocation(makeAFaceMainShader, "iTime");
				int referenceTexture = glGetUniformLocation(makeAFaceMainShader, "previousTexture");
				int trueTexture = glGetUniformLocation(makeAFaceMainShader, "trueTexture");

				//Settings
				int triangleSetting = glGetUniformLocation(makeAFaceMainShader, "useTriangles");
				int pixelColorSetting = glGetUniformLocation(makeAFaceMainShader, "everyPixelSameColor");
				int sourceColorSetting = glGetUniformLocation(makeAFaceMainShader, "sourceColors");

				glUseProgram(makeAFaceMainShader);
				glUniform2f(uniformWindowSize, WINDOW_WIDTH, WINDOW_HEIGHT);
				glUniform1f(uniformTime, ((float)currentTime));

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, referenceFBTexture);
				glUniform1i(referenceTexture, 0);

				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, cameraTexture);
				glUniform1i(trueTexture, 1);

				glUniform1i(triangleSetting, useTriangles);
				glUniform1i(pixelColorSetting, everyPixelSameColor);
				glUniform1i(sourceColorSetting, sourceColors);

				drawQuad();
			}

			//Copy current texture to previous texture
			{
				glBindFramebuffer(GL_FRAMEBUFFER, referenceFramebuffer);

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

			if (showImGuiWindow)
			{
				ImGui::Render();
			}

			glfwSwapBuffers(window);
		}
	}

	keepCapturing = false;
	captureThread.join();

	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	return 0;
}