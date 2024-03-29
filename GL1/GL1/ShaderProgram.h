#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

/// <summary>
/// ��ɫ���������(Shader Program Object)�Ƕ����ɫ���ϲ�֮������������ɵİ汾��
/// ���Ҫʹ�øղű������ɫ�����Ǳ������������(Link)Ϊһ����ɫ���������Ȼ������Ⱦ�����ʱ�򼤻������ɫ������
/// �Ѽ�����ɫ���������ɫ���������Ƿ�����Ⱦ���õ�ʱ��ʹ�á�
/// </summary>
class ShaderProgram
{
	public:
		unsigned int GetID() { return ID; }
		ShaderProgram()
		{
			// glCreateProgram��������һ�����򣬲������´�����������ID����
			ID = glCreateProgram();
		}

		/// <summary>
		/// ��ɫ�����ӵ����������
		/// </summary>
		/// <param name="shader"></param>
		void AttachShader(const Shader &shader)
		{
			
			glAttachShader(ID, shader.GetID());
		}

		/// <summary>
		/// ����
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
				// ��glGetShaderInfoLog��ȡ������Ϣ��Ȼ���ӡ
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			}
			return success;
		}

		/// <summary>
		/// ����������
		/// </summary>
		void Use()
		{
			glUseProgram(ID);
		}

	private:
		unsigned int ID;
};