#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <type_traits>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

#ifdef WIN32 // debug-breakpoint/platform-specific
#define BREAKPOINT() __debugbreak()
#elif defined (__unix__)  || (defined (__APPLE__) && defined (__MACH__))
#define BREAKPOINT() raise(SIGTRAP)
#else // see other methods: https://stackoverflow.com/questions/173618/is-there-a-portable-equivalent-to-debugbreak-debugbreak/49079078#49079078
#define BREAKPOINT() // uncomment if no solutions found
#endif // debug-breakpoint/platform-specific

// Trigger debug breakpoint on `condition` fail
#define ASSERT(condition) if (!(condition)) BREAKPOINT()

// Assert GL function return type value (signed != -1 && unsigned != 0u) // So genious -_- to not use unsigned everywhere // Metaprogramming == my love :)
#define GLASSERT(glretval) \
	static_assert(std::is_arithmetic_v<decltype(glretval)>); \
	if constexpr (std::is_signed_v    <decltype(glretval)>) { ASSERT(glretval != -1); } \
	else /* if (unsigned arithmetic type)                */ { ASSERT(glretval != 0u); }

static void GLAPIENTRY GlDebugMessage_cb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param)
{
	char *source_, *type_, *severity_;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
	case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
	default:                              source_ = "<SOURCE>";        break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
	case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
	case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
	case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
	default:                                type_ = "<TYPE>";              break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
	case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
	default:                             severity_ = "<SEVERITY>";   break;
	}

	std::cerr << std::setw(4) << id << ": GL " << severity_ << ' ' << type_ << " (" << source_ << "): " << message << std::endl;

	BREAKPOINT();
}

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::filesystem::path &filePath)
{
	enum class ShaderType { none = -1, vertex = 0, fragment = 1 };

	std::ifstream stream(filePath);
	if (!stream) exit(EXIT_FAILURE);
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
	unsigned int shader = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char *message = static_cast<char *>(alloca(length * sizeof(char)));
		glGetShaderInfoLog(shader, length, nullptr, message);

		std::cerr << "Error: Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader\n";
		std::cerr << message << std::endl;

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

static int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

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

	glfwSwapInterval(1);

	if (GLenum err = glewInit(); err != GLEW_OK)
		std::cerr << "Error: glewInit(): " << glewGetErrorString(err) << std::endl;

	// Enable OpenGL features
	glEnable(GL_DEBUG_OUTPUT);
	if (glIsEnabled(GL_DEBUG_OUTPUT) == GL_TRUE) // attemt to validate if gl-feature works
		glDebugMessageCallback(GlDebugMessage_cb, nullptr);

	std::cout << "Info: Vendor      : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Info: Renderer    : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Info: GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Info: GL   version: " << glGetString(GL_VERSION) << std::endl;

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

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<const void *>(0));

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	GLASSERT(location); //ASSERT(location != -1);
	glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);

	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform4f(location, r, 0.3f, 0.8f, 1.0f);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = +0.05f;
		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
