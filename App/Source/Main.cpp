#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>

#include "Shader.h"
#include "Renderer.h"

static uint32_t s_ComputeShader = -1;
static const std::filesystem::path s_ComputeShaderPath = "../App/Shaders/Compute.glsl";

static void ErrorCallback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_R)
		s_ComputeShader = ReloadComputeShader(s_ComputeShader, s_ComputeShaderPath);
}

int main()
{
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	int width = 1280;
	int height = 720;

	// Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	GLFWwindow* window = glfwCreateWindow((int)(width*main_scale), (int)(height*main_scale), "ImGui Compute Shader", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, KeyCallback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	bool vsync = true;
	glfwSwapInterval(vsync);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 440");

	s_ComputeShader = CreateComputeShader(s_ComputeShaderPath);
	if (s_ComputeShader == -1)
	{
		std::cerr << "Compute shader failed\n";
		return -1;
	}

	Texture computeShaderTexture = CreateTexture(width, height);
	Framebuffer fb = CreateFramebufferWithTexture(computeShaderTexture);
	
	while (!glfwWindowShouldClose(window))
	{

		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		// Settings
		{
			glfwGetWindowSize(window, &width, &height);

			ImGui::SetNextWindowPos(ImVec2(width-(width/3),0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(width/3, height), ImGuiCond_Always);
			ImGui::Begin("Invisible Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
			ImGui::End();

			ImGui::Begin("Settings", nullptr);
			//Settings Here
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			if (ImGui::Checkbox("VSync", &vsync)) {
				glfwSwapInterval(vsync);
			}
            ImGui::End();
		}


		ImGui::Render();
		glfwGetFramebufferSize(window, &width, &height);

		// Resize texture
		if (width != computeShaderTexture.Width || height != computeShaderTexture.Height)
		{
			glDeleteTextures(1, &computeShaderTexture.Handle);
			computeShaderTexture = CreateTexture(width, height);
			AttachTextureToFramebuffer(fb, computeShaderTexture);
		}

		// Compute
		{
			glUseProgram(s_ComputeShader);
			glBindImageTexture(0, fb.ColorAttachment.Handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			const GLuint workGroupSizeX = 16;
			const GLuint workGroupSizeY = 16;

			GLuint numGroupsX = (width + workGroupSizeX - 1) / workGroupSizeX;
			GLuint numGroupsY = (height + workGroupSizeY - 1) / workGroupSizeY;

			glDispatchCompute(numGroupsX, numGroupsY, 1);

			// Ensure all writes to the image are complete
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		// Blit
		{
			BlitFramebufferToSwapchain(fb);
		}
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
