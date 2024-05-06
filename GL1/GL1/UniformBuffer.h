#pragma once

#include <string>
#include <glad/glad.h>

/// <summary>
/// Uniform缓冲对象
/// </summary>
class UniformBuffer
{
	public:
		unsigned int ID = 0;
		/// <summary>
		/// 对应shader中Uniform块的名称
		/// </summary>
		std::string buffer_name;
		/// <summary>
		/// Uniform缓冲绑定点。
		/// 当Uniform缓冲对象与shader中的uniform块绑定到同一个绑定点上时，
		/// shader就可以读取这个Uniform缓冲对象持有的数据
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
		/// 创建Uniform缓冲对象 ，并将其绑定到绑定点上
		/// </summary>
		/// <param name="size">分配的内存大小</param>
		void GenBuffer(int size);

		/// <summary>
		/// 创建Uniform缓冲对象 ，并将其绑定到绑定点上
		/// </summary>
		/// <param name="size">分配的内存大小</param>
		void GenBuffer(int size, int offset);

		UniformBuffer() = default;
};




