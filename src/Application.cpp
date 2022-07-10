#include "Utility.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	GLFWwindow *window = glfwCreateWindow(640, 480, "ChernoOpenGL", NULL, NULL);
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

	std::cout << std::endl;

	std::cout << "Info: GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Info: GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Info: GPU  vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;

	{ // Vertex-/Index-Buffer scope
		float positions[] = { // pos[x,y...]
			-0.5f, -0.5f, // 0
			 0.5f, -0.5f, // 1
			 0.5f,  0.5f, // 2
			-0.5f,  0.5f, // 3
		};

		unsigned int indices[] = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};

		static_assert(sizeof(float) == sizeof(std::remove_extent_t<decltype(positions)>));
		VertexBuffer vb(positions, 4 * 2 * sizeof(std::remove_extent_t<decltype(positions)>));
		VertexArray va;
		VertexBufferLayout layout;

		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			renderer.Draw(va, ib, shader);

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = +0.05f;
			r += increment;

			glfwSwapBuffers(window);
			glfwPollEvents();

		} // while (!glfwWindowShouldClose(window))

	} // Vertex-/Index-Buffer scope

	glfwTerminate();
}
