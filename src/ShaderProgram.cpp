#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stb_include/stb_include.h>

ShaderProgram::ShaderProgram(const string& computePath) {
	shaderProgramID = LinkProgram(std::vector<unsigned int>{CompileShader(computePath, GL_COMPUTE_SHADER)});
}

unsigned int ShaderProgram::CompileShader(string shaderPath, GLenum type) {
	if (!(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_COMPUTE_SHADER)) {
		cout << "SHADER COMP ERROR: Shader at path" << endl;
		cout << shaderPath << " of type " << type << " is not supported." << endl;
		exit(-1);
	}

	string pathToIncludes = shaderPath.substr(0, shaderPath.find_last_of('/'));

	char error[256] = {'\0'};
	char* shaderString = stb_include_file(
		const_cast<char*>(shaderPath.c_str()), nullptr, const_cast<char*>(pathToIncludes.c_str()), error);

	if (!shaderString) {
		cout << error << endl;
		free(shaderString);
		exit(-1);
	}

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderString, nullptr);
	glCompileShader(shader);

	free(shaderString);
	ShaderCompilationErrorCheck(shader, shaderPath);

	return shader;
}
unsigned int ShaderProgram::LinkProgram(const vector<unsigned int>& shaders) {
	unsigned int shaderProgramID = glCreateProgram();
	
	for (unsigned int shader : shaders) glAttachShader(shaderProgramID, shader);
	
	glLinkProgram(shaderProgramID);
	ProgramLinkingErrorCheck(shaderProgramID);

	return shaderProgramID;
}
void ShaderProgram::ShaderCompilationErrorCheck(unsigned int shader, const string& shaderPath) {
	int success;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);

		std::cout << "SHADER COMP ERROR: Could not compile shader at path '" << shaderPath << "'. Error message:\n" << infoLog << std::endl;
		exit(-1);
	}
}
void ShaderProgram::ProgramLinkingErrorCheck(unsigned int shaderProgramID) {
	int success;

	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(shaderProgramID, 1024, NULL, infoLog);

		std::cout << "PROGRAM LINK ERROR: Could not link shader program. Error message:\n" << infoLog << std::endl;
		exit(-1);
	}
}