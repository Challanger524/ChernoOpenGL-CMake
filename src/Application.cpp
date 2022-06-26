#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

int main(void)
{
	GLFWwindow *window;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	glfwSetErrorCallback([](int error, const char *description) { std::cerr << "Error: " << description << std::endl; });

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (GLenum err = glewInit(); err != GLEW_OK)
		std::cerr << "Error: glewInit(): " << glewGetErrorString(err) << std::endl;

	std::cout << "Info: Vendor      : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
