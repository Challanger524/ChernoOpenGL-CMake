#include "Utility.hpp"
#include "Renderer.hpp"

#include "tests/Test.hpp"
#include "tests/Test-ClearColor.hpp"
#include "tests/Test-Texture2D.hpp"
#include "tests/Test-Batching.hpp"
#include "tests/Test-Batching-Textures.hpp"
#include "tests/Test-Batching-Textures-dynamic.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <iostream>


int main(void)
{
	// Init/Setup GLFW (window, contexts, OS messages processing)
	if (!glfwInit())
	{ std::cerr << "Error: glfwInit() fail\n"; return -1; }

	glfwSetErrorCallback([](int error, const char *description) { std::cerr << "Error: " << description << std::endl; });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{ glfwTerminate(); return -1; }

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	// Init GLEW (run-time OpenGL extensions loader)
	if (GLenum err = glewInit(); err != GLEW_OK)
	{ std::cerr << "Error: glewInit(): " << glewGetErrorString(err) << std::endl; glfwTerminate(); return -1; }

	// Enable OpenGL features
	GLCall(glEnable(GL_DEBUG_OUTPUT));
	if (glIsEnabled(GL_DEBUG_OUTPUT) == GL_TRUE) // attemt to validate if gl-feature works
		GLCall(glDebugMessageCallback(GlDebugMessage_cb, nullptr));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


	// Init ImGui (graphical user interface for C++)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(nullptr);

	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Info: GPU  vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Info: ImGui version: " << IMGUI_VERSION;
#ifdef IMGUI_HAS_VIEWPORT
	std::cout << " +viewport";
#endif
#ifdef IMGUI_HAS_DOCK
	std::cout << " +docking";
#endif
	std::cout << std::endl << std::endl;

	{ // Vertex-/Index-Buffer scope
		Renderer renderer;

		test::Test *currentTest = nullptr;
		test::TestMenu *testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestTexture2D>("Texture 2D");
		testMenu->RegisterTest<test::Batching>("Batching");
		testMenu->RegisterTest<test::BatchingTextures>("Batching Textures");
		testMenu->RegisterTest<test::BatchingTexturesDynamic>("Batching Textures (dynamic)");

		bool show_demo_window = false;
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentTest)
			{
				currentTest->OnUpdate();
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-"))
				{ // return to menu
					delete currentTest;
					currentTest = testMenu;
					// Dodge: GL_INVALID_VALUE error generated.
					//        <program> handle does not refer to an object generated by OpenGL.
					// on RenderPlatformWindowsDefault()->ImGui_ImplOpenGL3_RenderDrawData() which restores GL state after drawing: `glUseProgram(last_program);`
					GLCall(glUseProgram(0));
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			{ // Show a simple window that we create ourselves (use a Begin/End pair to created a named window)
				ImGui::Checkbox("Demo Window", &show_demo_window);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			if (show_demo_window) // Show the big demo window (documentation active samples)
				ImGui::ShowDemoWindow(&show_demo_window);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(window);
			}

			glfwSwapBuffers(window);
			glfwPollEvents();

		} // while (!glfwWindowShouldClose(window))

		delete currentTest;
		if (currentTest != testMenu)
			delete testMenu;

	} // Vertex-/Index-Buffer scope


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
