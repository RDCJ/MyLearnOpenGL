#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

enum ShaderType
{
	Vertex,
	Fragment
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
		/// ���Shader�����Ƿ����ɹ�
		/// </summary>
		/// <returns></returns>
		bool CheckCompileSuccess();

	private:
		unsigned int ID = 0;
		void Compile(const char** shader_source);
};

#endif