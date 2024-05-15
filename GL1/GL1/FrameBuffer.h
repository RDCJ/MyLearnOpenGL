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
		/// 用帧缓冲的大小更新Viewport
		/// </summary>
		void UpdateViewport()
		{
			glViewport(0, 0, buffer_width, buffer_height);
		}

		/// <summary>
		/// glBindFramebuffer: 绑定帧缓冲, 绑定到GL_FRAMEBUFFER目标之后，所有的读取和写入帧缓冲的操作将会影响当前绑定的帧缓冲
		/// </summary>
		/// <param name="target"></param>
		/// <param name="_ID"></param>
		void Bind(GLenum target, unsigned int _ID) override
		{
			glBindFramebuffer(target, _ID);
		}

		/// <summary>
		/// 检查帧缓冲是否是完整的，如果不是，将打印错误信息。
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
		/// 渲染缓冲对象
		/// 渲染缓冲对象的数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过
		/// 当写入或者复制它的数据到其它缓冲中时是非常快的。所以，交换缓冲这样的操作在使用渲染缓冲对象时会非常快
		/// 渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问)
		/// 由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，只关心深度和模板测试
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



