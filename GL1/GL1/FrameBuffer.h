#pragma once

#include <glad/glad.h>
#include "Texture2D.h"
#include "TextureCubeMap.h"

class FrameBuffer: public GLObject
{
	public:
		FrameBuffer(){ }
		FrameBuffer(int buffer_width, int buffer_height);
		void AddTexture2D(GLenum internal_format, GLenum format, GLenum data_type, int mipmap_level= 0, const TexParams& params=TexParams());
		void AddTextureCubMap(GLenum format, GLenum data_type, int mipmap_level = 0, const TexParams& params = TexParams());
		void AddRenderBuffer();
		/// <summary>
		/// ��֡����Ĵ�С����Viewport
		/// </summary>
		void UpdateViewport()
		{
			glViewport(0, 0, buffer_width, buffer_height);
		}

		/// <summary>
		/// glBindFramebuffer: ��֡����, �󶨵�GL_FRAMEBUFFERĿ��֮�����еĶ�ȡ��д��֡����Ĳ�������Ӱ�쵱ǰ�󶨵�֡����
		/// </summary>
		/// <param name="target"></param>
		/// <param name="_ID"></param>
		void Bind(GLenum target, unsigned int _ID) override
		{
			glBindFramebuffer(target, _ID);
		}

		/// <summary>
		/// ���֡�����Ƿ��������ģ�������ǣ�����ӡ������Ϣ��
		/// </summary>
		void CheckStatus();

		int buffer_width = 0;
		int buffer_height = 0;

		Texture2D* color_buffer = nullptr;
		TextureCubeMap* cube_map_buffer = nullptr;

		void BindTexture2D(Texture2D texture, GLenum attachment, int mipmap_level)
		{
			glFramebufferTexture2D(GLTarget(), attachment, GL_TEXTURE_2D, texture.GetID(), mipmap_level);
		}

	private:
		/// <summary>
		/// ��Ⱦ�������
		/// ��Ⱦ�����������ݴ���ΪOpenGLԭ������Ⱦ��ʽ������Ϊ������Ⱦ��֡�����Ż���
		/// ��д����߸����������ݵ�����������ʱ�Ƿǳ���ġ����ԣ��������������Ĳ�����ʹ����Ⱦ�������ʱ��ǳ���
		/// ��Ⱦ�������ͨ������ֻд�ģ������㲻�ܶ�ȡ���ǣ�����ʹ���������)
		/// ������Ⱦ�������ͨ������ֻд�ģ����ǻᾭ��������Ⱥ�ģ�帽������Ϊ�󲿷�ʱ�����Ƕ�����Ҫ����Ⱥ�ģ�建���ж�ȡֵ��ֻ������Ⱥ�ģ�����
		/// </summary>
		unsigned int RBO = 0;

		
		GLenum GLTarget() override
		{
			return GL_FRAMEBUFFER;
		}
		void Generate() override
		{
			glGenFramebuffers(1, &ID);
		}
};



