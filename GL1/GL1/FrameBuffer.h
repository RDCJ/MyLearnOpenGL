#pragma once

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include "Texture2D.h"

class FrameBuffer
{
	public:
		FrameBuffer(int buffer_width, int buffer_height);
		void AddTexture(GLenum format, GLenum data_type, int mipmap_level= 0, const TexParams& params=TexParams());
		void AddRenderBuffer();
		/// <summary>
		/// 用帧缓冲的大小更新Viewport
		/// </summary>
		void UpdateViewport()
		{
			glViewport(0, 0, buffer_width, buffer_height);
		}
		void Bind();

		int buffer_width;
		int buffer_height;

		Texture2D* color_buffer = nullptr;

	private:
		unsigned int ID;
		/// <summary>
		/// 渲染缓冲对象
		/// 渲染缓冲对象的数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过
		/// 当写入或者复制它的数据到其它缓冲中时是非常快的。所以，交换缓冲这样的操作在使用渲染缓冲对象时会非常快
		/// 渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问)
		/// 由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，只关心深度和模板测试
		/// </summary>
		unsigned int RBO = 0;
};

#endif // !FRAMEBUFFER_H



