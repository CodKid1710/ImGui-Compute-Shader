#include "Shader.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <glad/gl.h>



uint32_t CreateComputeShader(const std::filesystem::path& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << path.string() << std::endl;
		return -1;
	}

	std::ostringstream contentStream;
	contentStream << file.rdbuf();
	std::string shaderSource = contentStream.str();

	GLuint shaderHandle = glCreateShader(GL_COMPUTE_SHADER);

	const GLchar* source = (const GLchar*)shaderSource.c_str();
	glShaderSource(shaderHandle, 1, &source, 0);

	glCompileShader(shaderHandle);

	GLint isCompiled = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shaderHandle, maxLength, &maxLength, &infoLog[0]);

		std::cerr << infoLog.data() << std::endl;

		glDeleteShader(shaderHandle);
		return -1;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderHandle);
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		
		std::cerr << infoLog.data() << std::endl;

		glDeleteProgram(program);
		glDeleteShader(shaderHandle);

		return -1;
	}

	glDetachShader(program, shaderHandle);
	return program;
}

uint32_t ReloadComputeShader(uint32_t shaderHandle, const std::filesystem::path& path)
{
	uint32_t newShaderHandle = CreateComputeShader(path);

	// Return old shader if compilation failed
	if (newShaderHandle == -1)
		return shaderHandle;

	glDeleteProgram(shaderHandle);
	return newShaderHandle;
}
