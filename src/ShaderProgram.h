#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

using namespace std;

namespace Atrium{
	class ShaderProgram {
	public:
		ShaderProgram();
		ShaderProgram(const string& computePath);
		ShaderProgram(const string& vertPath, const string& fragPath);

		void Use() const;
		static void UnUse();
	public:
		void SetMat4(const std::string& uniformName, glm::mat4 value) const;
		void SetInt(const std::string& uniformName, int value) const;
		void SetUint(const std::string& uniformName, unsigned int value) const;
		void BindStorageBlock(std::string blockName, unsigned int bindPoint) const;
	private:
		unsigned int CompileShader(string shaderPath, GLenum type);
		unsigned int LinkProgram(const vector<unsigned int>& shaders);

		void ShaderCompilationErrorCheck(unsigned int shader, const string& shaderPath);
		void ProgramLinkingErrorCheck(unsigned int shaderProgramID);
	private:
		unsigned int shaderProgramID;
	};
}

