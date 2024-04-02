#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		ShaderProgram(const char* vert_source_path, const char* frag_source_path);

		~ShaderProgram()
		{
			glDeleteProgram(ID);
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

		bool CheckLinkSuccess();

		/// <summary>
		/// 激活程序对象
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
			//第二个参数告诉OpenGL我们将要发送多少个矩阵。
			// 第三个参数表示是否对矩阵进行转置(Transpose)。OpenGL通常使用列主序(Column-major Ordering)布局。GLM的默认布局就是列主序，所以通常并不需要转置矩阵。
			// 最后一个参数是真正的矩阵数据，但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，因此我们要先用GLM的自带的函数value_ptr来变换这些数据
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