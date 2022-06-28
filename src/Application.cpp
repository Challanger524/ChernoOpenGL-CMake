#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <type_traits>


#ifdef _WIN32 // debug-breakpoint/platform-specific
#include <intrin.h>
#define BREAKPOINT() __debugbreak()
#elif defined (__unix__)  || (defined (__APPLE__) && defined (__MACH__))
#include <signal.h>
#define BREAKPOINT() raise(SIGTRAP)
#else // see other methods: https://stackoverflow.com/questions/173618/is-there-a-portable-equivalent-to-debugbreak-debugbreak/49079078#49079078
//#define BREAKPOINT() // uncomment if no solutions found
#endif // debug-breakpoint/platform-specific

// Trigger debug breakpoint on `condition` fail
#define ASSERT(condition) if (!(condition)) BREAKPOINT()

// Assert GL function return type value (signed != -1 && unsigned != 0u) // So genious -_- to not use unsigned everywhere // Metaprogramming == my love :)
#define GLASSERT(glretval) \
	static_assert(std::is_arithmetic_v<decltype(glretval)>); \
	if constexpr (std::is_signed_v    <decltype(glretval)>) { ASSERT(glretval != -1); } \
	else /* if (unsigned arithmetic type)                */ { ASSERT(glretval != 0u); }

#define GLCall(function_call) \
    while (glGetError() != GL_NO_ERROR) /* pop */;\
    function_call;\
    if (glGetError() != GL_NO_ERROR) {\
		std::cerr << __FILE__ << ':' << __LINE__ << ": " << #function_call << "\n\n";\
		BREAKPOINT(); } // <-- scope_end}


static void GLAPIENTRY GlDebugMessage_cb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *)
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
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; return; //break;
	default:                             severity_ = "<SEVERITY>";   break;
	}

	std::cerr << std::setw(4) << id << ": GL " << severity_ << ' ' << type_ << " (" << source_ << "): " << message << std::endl;

	// Do not `Breakpoint()` here since callstack is missing/wired/untraceble for Nvidia drivers
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

	glfwSetErrorCallback([](int, const char *description) { std::cerr << "Error: " << description << std::endl; });

	GLFWwindow *window = glfwCreateWindow(640, 480, "ChernoOpenGL", NULL, NULL);
	if (!window)
	{ glfwTerminate(); return -1; }

	glfwMakeContextCurrent(window);


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
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<const void *>(0)));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
	GLCall(glUseProgram(shader));

	while (!glfwWindowShouldClose(window))
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		glfwSwapBuffers(window);
		glfwPollEvents();

	} // while (!glfwWindowShouldClose(window))

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
}
