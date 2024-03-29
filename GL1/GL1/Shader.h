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
			// ����һ����ɫ������ע������ID�����õ�
			switch (shaderType)
			{
				case ShaderType::Vertex:
					ID = glCreateShader(GL_VERTEX_SHADER);
					break;
				case ShaderType::Fragment:
					ID = glCreateShader(GL_FRAGMENT_SHADER);
					break;
			}
			// ����ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ�������
			glShaderSource(ID, 1, shader_source, NULL);
			glCompileShader(ID);
		}

		~Shader()
		{
			glDeleteShader(ID);
		}

		/// <summary>
		/// ���Shader�����Ƿ����ɹ�
		/// </summary>
		/// <returns></returns>
		bool CheckCompileSuccess()
		{
			int success;
			// ��glGetShaderiv����Ƿ����ɹ�
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				char infoLog[512];
				// ��glGetShaderInfoLog��ȡ������Ϣ��Ȼ���ӡ
				glGetShaderInfoLog(ID, 512, NULL, infoLog);
				std::cout<< "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			return success;
		}

	private:
		unsigned int ID;

};