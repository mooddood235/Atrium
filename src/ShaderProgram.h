#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>

using namespace std;

class ShaderProgram{
public:
	ShaderProgram(const string& computePath);
private:
	unsigned int CompileShader(string shaderPath, GLenum type);
	unsigned int LinkProgram(const vector<unsigned int>& shaders);

	void ShaderCompilationErrorCheck(unsigned int shader, const string& shaderPath);
	void ProgramLinkingErrorCheck(unsigned int shaderProgramID);
private:
	unsigned int shaderProgramID;
};

