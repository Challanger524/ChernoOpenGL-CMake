#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
	// Init/Setup GLFW (window, contexts, OS messages processing)
	if (!glfwInit())
	{ std::cerr << "Error: glfwInit() fail\n"; return -1; }

	glfwSetErrorCallback([](int, const char *description) { std::cerr << "Error: " << description << std::endl; });

	GLFWwindow *window = glfwCreateWindow(640, 480, "ChernoOpenGL", NULL, NULL);
	if (!window)
	{ glfwTerminate(); return -1; }

	glfwMakeContextCurrent(window);


	// Init GLEW (run-time OpenGL extensions loader)
	if (GLenum err = glewInit(); err != GLEW_OK)
	{ std::cerr << "Error: glewInit(): " << glewGetErrorString(err) << std::endl; glfwTerminate(); return -1; }

	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Info: GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Info: GPU  vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} // while (!glfwWindowShouldClose(window))

	glfwTerminate();
}
