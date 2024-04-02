#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		ShaderProgram(const char* vert_source_path, const char* frag_source_path);

		~ShaderProgram()
		{
			glDeleteProgram(ID);
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

		bool CheckLinkSuccess();

		/// <summary>
		/// ����������
		/// </summary>
		void Use()
		{
			glUseProgram(ID);
		}

		void SetUniformBool(const std::string& name, bool value) const
		{
			glUniform1i(GetUniformLocation(name.c_str()), (int)value);
		}

		void SetUniformInt(const std::string& name, int value) const
		{
			glUniform1i(GetUniformLocation(name.c_str()), value);
		}

		void SetUniformFloat(const std::string& name, float value) const
		{
			glUniform1f(GetUniformLocation(name.c_str()), value);
		}

		void SetUniformMat4f(const std::string& name, glm::mat4& mat, bool transpose=false) const
		{
			//�ڶ�����������OpenGL���ǽ�Ҫ���Ͷ��ٸ�����
			// ������������ʾ�Ƿ�Ծ������ת��(Transpose)��OpenGLͨ��ʹ��������(Column-major Ordering)���֡�GLM��Ĭ�ϲ��־�������������ͨ��������Ҫת�þ���
			// ���һ�������������ľ������ݣ�����GLM�����ǰ����ǵľ��󴢴�ΪOpenGL��ϣ�����ܵ����֣��������Ҫ����GLM���Դ��ĺ���value_ptr���任��Щ����
			glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, (int)transpose, glm::value_ptr(mat));
		}

		void SetUniformVec3(const std::string& name, glm::vec3& vec) const
		{
			glUniform3f(GetUniformLocation(name.c_str()), vec.x, vec.y, vec.z);
		}

		void SetUniformVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(GetUniformLocation(name.c_str()), x, y, z);
		}

		int GetUniformLocation(const char* name) const
		{
			return glGetUniformLocation(ID, name);
		}

	private:
		unsigned int ID;
};