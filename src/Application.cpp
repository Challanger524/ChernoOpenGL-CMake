#include "Utility.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <type_traits>


int main(void)
{
	// Init/Setup GLFW (window, contexts, OS messages processing)
	if (!glfwInit())
	{ std::cerr << "Error: glfwInit() fail\n"; return -1; }

	glfwSetErrorCallback([](int, const char *description) { std::cerr << "Error: " << description << std::endl; });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(960, 540, "ChernoOpenGL", NULL, NULL);
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

	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Info: GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Info: GPU  vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;

	{ // Vertex-/Index-Buffer scope
		float positions[] = { // pos[x,y...]
			100.0f, 100.0f, 0.0f, 0.0f, // 0
			200.0f, 100.0f, 1.0f, 0.0f, // 1
			200.0f, 200.0f, 1.0f, 1.0f, // 2
			100.0f, 200.0f, 0.0f, 1.0f, // 3
		};

		unsigned int indices[] = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};

		static_assert(sizeof(float) == sizeof(std::remove_extent_t<decltype(positions)>));
		VertexBuffer vb(positions, 4 * 4 * sizeof(std::remove_extent_t<decltype(positions)>));
		VertexArray va;
		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0)); // move model to the up-right
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(100, 0, 0)); // move camera to the left (world to the right)

		glm::mat4 mvp = proj * model * view;

		Texture texture("res/textures/ChernoLogo.png");
		texture.Bind(0);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform1i("u_Texture", 0);
		shader.SetUniformMat4f("u_MVP", mvp);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			shader.Bind();

			renderer.Draw(va, ib, shader);

			glfwSwapBuffers(window);
			glfwPollEvents();

		} // while (!glfwWindowShouldClose(window))

	} // Vertex-/Index-Buffer scope

	glfwTerminate();
}
