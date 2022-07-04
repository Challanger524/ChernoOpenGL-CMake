#include "Utility.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <type_traits>

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::filesystem::path &filePath)
{
	enum class ShaderType { none = -1, vertex = 0, fragment = 1 };

	std::ifstream stream(filePath);
	if (!stream)
	{
		std::cerr << "Error: Fail to open shader path: " << filePath << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::none;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::vertex;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::fragment;
		}
		else
		{
			ss[static_cast<int>(type)] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string &source)
{
	unsigned int shader;
	GLCall(shader = glCreateShader(type));

	const char *src = source.c_str();
	GLCall(glShaderSource(shader, 1, &src, nullptr));
	GLCall(glCompileShader(shader));

	int result;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
		char *message = static_cast<char *>(alloca(length * sizeof(char)));
		GLCall(glGetShaderInfoLog(shader, length, nullptr, message));

		std::cerr << "Error: Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader\n";
		std::cerr << message << std::endl;

		GLCall(glDeleteShader(shader));
		return 0;
	}

	return shader;
}

static int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	unsigned int program;
	GLCall(program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}


int main(void)
{
	// Init/Setup GLFW (window, contexts, OS messages processing)
	if (!glfwInit())
	{ std::cerr << "Error: glfwInit() fail\n"; return -1; }

	glfwSetErrorCallback([](int error, const char *description) { std::cerr << "Error: " << description << std::endl; });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		static_assert(sizeof(float) == sizeof(std::remove_extent_t<decltype(positions)>));
		VertexBuffer vb(positions, 4 * 2 * sizeof(std::remove_extent_t<decltype(positions)>));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<const void *>(0))); // links `buffer` to `vao`

		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
		GLCall(glUseProgram(shader));

		int location;
		GLCall(location = glGetUniformLocation(shader, "u_Color"));
		GLASSERT(location); //ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

		GLCall(glBindVertexArray(0));
		vb.Unbind();
		ib.Unbind();
		GLCall(glUseProgram(0));

		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			GLCall(glBindVertexArray(vao));
			ib.Bind();

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = +0.05f;
			r += increment;

			glfwSwapBuffers(window);
			glfwPollEvents();

		} // while (!glfwWindowShouldClose(window))

		GLCall(glDeleteProgram(shader));

	} // Vertex-/Index-Buffer scope

	glfwTerminate();
}
