#pragma once

#include <GL/glew.h>

#include <iostream>
#include <iomanip>


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


static void GLAPIENTRY GlDebugMessage_cb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param)
{
	const char *source_, *type_, *severity_;

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
