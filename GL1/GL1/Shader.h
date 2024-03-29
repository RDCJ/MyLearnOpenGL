#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

		Shader(const char** shader_source, ShaderType _shadetType)
		{
			shaderType = _shadetType;
			// 创建一个着色器对象，注意是用ID来引用的
			switch (shaderType)
			{
				case ShaderType::Vertex:
					ID = glCreateShader(GL_VERTEX_SHADER);
					break;
				case ShaderType::Fragment:
					ID = glCreateShader(GL_FRAGMENT_SHADER);
					break;
			}
			// 把着色器源码附加到着色器对象上，然后编译它
			glShaderSource(ID, 1, shader_source, NULL);
			glCompileShader(ID);
		}

		~Shader()
		{
			glDeleteShader(ID);
		}

		/// <summary>
		/// 检查Shader代码是否编译成功
		/// </summary>
		/// <returns></returns>
		bool CheckCompileSuccess()
		{
			int success;
			// 用glGetShaderiv检查是否编译成功
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				char infoLog[512];
				// 用glGetShaderInfoLog获取错误消息，然后打印
				glGetShaderInfoLog(ID, 512, NULL, infoLog);
				std::cout<< "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			return success;
		}

	private:
		unsigned int ID;

};