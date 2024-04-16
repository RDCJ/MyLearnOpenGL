#pragma once

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include "Texture2D.h"

class FrameBuffer
{
	public:
		FrameBuffer(int buffer_width, int buffer_height);
		void AddTexture(GLenum format=GL_RGB, int mipmap_level= 0);
		void AddRenderBuffer();
		/// <summary>
		/// ��֡����Ĵ�С����Viewport
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
		/// ��Ⱦ�������
		/// ��Ⱦ�����������ݴ���ΪOpenGLԭ������Ⱦ��ʽ������Ϊ������Ⱦ��֡�����Ż���
		/// ��д����߸����������ݵ�����������ʱ�Ƿǳ���ġ����ԣ��������������Ĳ�����ʹ����Ⱦ�������ʱ��ǳ���
		/// ��Ⱦ�������ͨ������ֻд�ģ������㲻�ܶ�ȡ���ǣ�����ʹ����������)
		/// ������Ⱦ�������ͨ������ֻд�ģ����ǻᾭ��������Ⱥ�ģ�帽������Ϊ�󲿷�ʱ�����Ƕ�����Ҫ����Ⱥ�ģ�建���ж�ȡֵ��ֻ������Ⱥ�ģ�����
		/// </summary>
		unsigned int RBO = 0;
};

#endif // !FRAMEBUFFER_H


