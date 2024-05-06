#pragma once

#include <glad/glad.h>
#include "Texture2D.h"
#include "TextureCubeMap.h"

class FrameBuffer
{
	public:
		FrameBuffer(int buffer_width, int buffer_height);
		void AddTexture2D(GLenum format, GLenum data_type, int mipmap_level= 0, const TexParams& params=TexParams());
		void AddTextureCubMap(GLenum format, GLenum data_type, int mipmap_level = 0, const TexParams& params = TexParams());
		void AddRenderBuffer();
		/// <summary>
		/// ��֡����Ĵ�С����Viewport
		/// </summary>
		void UpdateViewport()
		{
			glViewport(0, 0, buffer_width, buffer_height);
		}
		void Bind();
		/// <summary>
		/// ���֡�����Ƿ��������ģ�������ǣ�����ӡ������Ϣ��
		/// </summary>
		void CheckStatus();

		int buffer_width;
		int buffer_height;

		Texture2D* color_buffer = nullptr;
		TextureCubeMap* cube_map_buffer = nullptr;

	private:
		unsigned int ID;
		/// <summary>
		/// ��Ⱦ�������
		/// ��Ⱦ�����������ݴ���ΪOpenGLԭ������Ⱦ��ʽ������Ϊ������Ⱦ��֡�����Ż���
		/// ��д����߸����������ݵ�����������ʱ�Ƿǳ���ġ����ԣ��������������Ĳ�����ʹ����Ⱦ�������ʱ��ǳ���
		/// ��Ⱦ�������ͨ������ֻд�ģ������㲻�ܶ�ȡ���ǣ�����ʹ���������)
		/// ������Ⱦ�������ͨ������ֻд�ģ����ǻᾭ��������Ⱥ�ģ�帽������Ϊ�󲿷�ʱ�����Ƕ�����Ҫ����Ⱥ�ģ�建���ж�ȡֵ��ֻ������Ⱥ�ģ�����
		/// </summary>
		unsigned int RBO = 0;
};



