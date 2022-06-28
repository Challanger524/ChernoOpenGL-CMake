#pragma once

#include <GL/glew.h>

#include <iostream>
#include <iomanip>
#include <type_traits>

// Compiler-friendly debug breakpoints (in source code)
// https://stackoverflow.com/questions/173618/is-there-a-portable-equivalent-to-debugbreak-debugbreak/49079078#49079078
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2514r0.html#_implementation_experience
//#if defined(_WIN64) || defined(_MSC_VER) || defined(__INTEL_COMPILER) // msvc
#if !defined(BREAKPOINT) && __has_include(<intrin.h>)
#  include <intrin.h>
#  define BREAKPOINT() __debugbreak()
#elif defined(__has_builtin) && !defined(__ibmxl__) // clang/gcc
#  if __has_builtin(__builtin_debugtrap) // clang
#    define BREAKPOINT() __builtin_debugtrap()
//#  elif __has_builtin(__builtin_trap) // gcc
//#    define BREAKPOINT() __builtin_trap() // not much useful
#  endif
#endif
//#if !defined(BREAKPOINT) && (defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))) // other
#if !defined(BREAKPOINT) && __has_include(<signal.h>)
#  include <signal.h>
#  if defined(SIGTRAP)
#    define BREAKPOINT() raise(SIGTRAP)
#  else
#    define BREAKPOINT() raise(SIGABRT)
#  endif
#endif

// Trigger debug breakpoint on `condition` fail
#define ASSERT(condition) if (!(condition)) BREAKPOINT()

// Assert GL function return (un)signed type value (signed != -1 && unsigned != 0u) // Metaprogramming == my love ❤️
#define GLASSERT(glretval) \
	static_assert(std::is_arithmetic_v<decltype(glretval)>); \
	if constexpr (std::is_signed_v    <decltype(glretval)>) { ASSERT(glretval != -1); } \
	else /* if   (unsigned_arithmetic_type)              */ { ASSERT(glretval != 0u); }

#define GLCall(function_call) \
	while (glGetError() != GL_NO_ERROR) /* pop */;\
	function_call;\
	if (glGetError() != GL_NO_ERROR) {\
		std::cerr << __FILE__ << ':' << __LINE__ << ": " << #function_call << "\n\n";\
		BREAKPOINT();\
	}

// Forked and polished gist: https://gist.github.com/Challanger524/cdf90cf11809749363fb638646225773
static void GLAPIENTRY GlDebugMessage_cb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *)
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

	std::cerr << std::setw(4) << id << ": " << severity_ << " / " << type_ << " / " << source_ << ": " << message << std::endl;

	//BREAKPOINT() // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS) must be enabled
}
