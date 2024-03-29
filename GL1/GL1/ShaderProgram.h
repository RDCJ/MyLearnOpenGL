#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

/// <summary>
/// 着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本。
/// 如果要使用刚才编译的着色器我们必须把它们链接(Link)为一个着色器程序对象，然后在渲染对象的时候激活这个着色器程序。
/// 已激活着色器程序的着色器将在我们发送渲染调用的时候被使用。
/// </summary>
class ShaderProgram
{
	public:
		unsigned int GetID() { return ID; }
		ShaderProgram()
		{
			// glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用
			ID = glCreateProgram();
		}

		/// <summary>
		/// 着色器附加到程序对象上
		/// </summary>
		/// <param name="shader"></param>
		void AttachShader(const Shader &shader)
		{
			
			glAttachShader(ID, shader.GetID());
		}

		/// <summary>
		/// 链接
		/// </summary>
		void Link()
		{
			glLinkProgram(ID);
		}

		bool CheckLinkSuccess()
		{
			int success;
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if (!success)
			{
				char infoLog[512];
				// 用glGetShaderInfoLog获取错误消息，然后打印
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			}
			return success;
		}

		/// <summary>
		/// 激活程序对象
		/// </summary>
		void Use()
		{
			glUseProgram(ID);
		}

	private:
		unsigned int ID;
};