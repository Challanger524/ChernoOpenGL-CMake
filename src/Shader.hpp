#pragma once

#include "Utility.hpp"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
	unsigned int m_RendererId = 0;
	std::filesystem::path m_filePath;
	mutable std::unordered_map<std::string, int> m_locationCache;

public:
	Shader(const std::filesystem::path &filepath) : m_filePath(filepath)
	{
		ShaderProgramSource source = ParseShader(m_filePath);
		m_RendererId = CreateShader(source.vertexSource, source.fragmentSource);
	}
	~Shader() { GLCall(glDeleteProgram(m_RendererId)); }

	void Bind() const { GLCall(glUseProgram(m_RendererId)); }
	void Unbind() const { GLCall(glUseProgram(0)); }

	void SetUniform1i(const std::string &name, int value) /*                        */ { GLCall(glUniform1i(GetUniformLocation(name), value)); }
	void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) { GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); }

private:
	ShaderProgramSource ParseShader(const std::filesystem::path &filePath)
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

	unsigned int CompileShader(unsigned int type, const std::string &source)
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

	unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
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

	int GetUniformLocation(const std::string &name) const
	{
		if (const auto cache = m_locationCache.find(name); cache != m_locationCache.end())
			return cache->second;

		int location;
		GLCall(location = glGetUniformLocation(m_RendererId, name.c_str()));
		m_locationCache[name] = location;

		if (location == -1) std::cerr << "Warning: uniform '" << name << "' doesn't exist!\n";

		return location;
	}
};
