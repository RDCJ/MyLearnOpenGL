#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

enum ShaderType
{
	VertexShader,
	GeometryShader,
	FragmentShader
};

class Shader
{
	public:
		ShaderType shaderType;
		int GetID() const { return ID; }

		Shader(const char** shader_source, ShaderType _shaderType);

		Shader(const char* source_path, ShaderType _shaderType);
		
		~Shader()
		{
			glDeleteShader(ID);
		}


		/// <summary>
		/// 检查Shader代码是否编译成功
		/// </summary>
		/// <returns></returns>
		bool CheckCompileSuccess();

	private:
		unsigned int ID = 0;
		void Compile(const char** shader_source);
};
