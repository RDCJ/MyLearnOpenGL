#pragma once

#include <string>
#include <glad/glad.h>

/// <summary>
/// Uniform�������
/// </summary>
class UniformBuffer
{
	public:
		unsigned int ID = 0;
		/// <summary>
		/// ��Ӧshader��Uniform�������
		/// </summary>
		std::string buffer_name;
		/// <summary>
		/// Uniform����󶨵㡣
		/// ��Uniform���������shader�е�uniform��󶨵�ͬһ���󶨵���ʱ��
		/// shader�Ϳ��Զ�ȡ���Uniform���������е�����
		/// </summary>
		unsigned int binding_point;

		UniformBuffer(const std::string& buffer_name, int binding_point):buffer_name(buffer_name), binding_point(binding_point)
		{

		}

		void Bind()
		{
			glBindBuffer(GL_UNIFORM_BUFFER, ID);
		}

		/// <summary>
		/// ����Uniform������� ��������󶨵��󶨵���
		/// </summary>
		/// <param name="size">������ڴ��С</param>
		void GenBuffer(int size);

		/// <summary>
		/// ����Uniform������� ��������󶨵��󶨵���
		/// </summary>
		/// <param name="size">������ڴ��С</param>
		void GenBuffer(int size, int offset);

		UniformBuffer() = default;
};




