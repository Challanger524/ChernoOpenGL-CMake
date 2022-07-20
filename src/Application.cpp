#include "Utility.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include "tests/Test.hpp"
#include "tests/Test-ClearColor.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <iostream>
#include <type_traits>


int main(void)
{
	// Init/Setup GLFW (window, contexts, OS messages processing)
	glfwSetErrorCallback([](int, const char *description) { std::cerr << "Error: " << description << std::endl; });

	if (!glfwInit())
	{ std::cerr << "Error: glfwInit() fail\n"; return -1; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE       , GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(960, 540, "ChernoOpenGL", NULL, NULL);
	if (!window)
	{ glfwTerminate(); return -1; }

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	// Init GLEW (run-time OpenGL extensions loader)
	if (GLenum err = glewInit(); err != GLEW_OK)
	{ std::cerr << "Error: glewInit(): " << glewGetErrorString(err) << std::endl; glfwTerminate(); return -1; }

	// Check if Debug Context enabled
	GLint glContextFlags; glGetIntegerv(GL_CONTEXT_FLAGS, &glContextFlags);
	if (glContextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
		std::cout << "Info: Context: Debug - active (supported)\n";
	else
		std::cout << "Info: Context: Debug - inactive (or unsupported)\n";

	// Enable OpenGL extensions and capabilities
	// extension: (for vendor) debug output
	if (GLEW_KHR_debug)
	{
		ASSERT(glfwExtensionSupported("GL_KHR_debug") == GLFW_TRUE);
		ASSERT(glewIsSupported       ("GL_KHR_debug") == GL_TRUE);
		ASSERT(glDebugMessageCallback != nullptr);

		GLCall(glEnable(GL_DEBUG_OUTPUT));
		GLCall(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
		GLCall(glDebugMessageCallback(GlDebugMessage_cb, nullptr));
		//GLCall(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));
		std::cout << "Info: GL: Extension: GL_KHR_debug - OK\n";
	}

	// capability: blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	std::cout << std::endl;

	// Init ImGui (graphical user interface for C++)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(nullptr);

	std::cout << "Info: GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Info: GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Info: GPU  vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Info: ImGui version: " << IMGUI_VERSION;
#ifdef IMGUI_HAS_DOCK
	std::cout << " +docking";
#endif
#ifdef IMGUI_HAS_VIEWPORT
	std::cout << " +viewport";
#endif
	std::cout << std::endl << std::endl;

	{ // Vertex-/Index-Buffer scope
		Renderer renderer;

		test::Test *currentTest = nullptr;
		test::TestMenu *testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");

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

		if (currentTest != testMenu)
			delete testMenu;
		delete currentTest;

	} // Vertex-/Index-Buffer scope


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
