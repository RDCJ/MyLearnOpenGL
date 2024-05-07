#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int buffer_width, int buffer_height): buffer_width(buffer_width), buffer_height(buffer_height)
{
	// 创建一个帧缓冲对象
	Generate();
}

void FrameBuffer::AddTexture2D(GLenum format, GLenum data_type, int mipmap_level, const TexParams& params)
{
	// 我们给纹理的data参数传递了NULL, 对于这个纹理，我们仅仅分配了内存而没有填充它。填充这个纹理将会在我们渲染到帧缓冲之后来进行
	color_buffer = new Texture2D(buffer_width, buffer_height, format, NULL, data_type);

	color_buffer->SetParameters(params);

	GLenum attachment;
	if (format == GL_DEPTH_COMPONENT)
		attachment = GL_DEPTH_ATTACHMENT;
	else
		attachment = GL_COLOR_ATTACHMENT0;

	BindSelf();
	// 将纹理附加到帧缓冲上， 之后所有的渲染指令将会写入到这个纹理中
	// glFramebufferTexture2D参数：
	// target：帧缓冲的目标（绘制、读取或者两者皆有）
	// attachment：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的0意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
	//	textarget：你希望附加的纹理类型
	// texture：要附加的纹理id
	//	level：mipmap的级别
	glFramebufferTexture2D(GLTarget(), attachment, GL_TEXTURE_2D, color_buffer->GetID(), mipmap_level);
	color_buffer->UnbindSelf();
	UnbindSelf();
}

void FrameBuffer::AddTextureCubMap(GLenum format, GLenum data_type, int mipmap_level, const TexParams& params)
{
	cube_map_buffer = new TextureCubeMap(buffer_width, buffer_height, format, NULL, data_type);

	cube_map_buffer->SetParameters(params);

	GLenum attachment;
	if (format == GL_DEPTH_COMPONENT)
		attachment = GL_DEPTH_ATTACHMENT;
	else
		attachment = GL_COLOR_ATTACHMENT0;

	BindSelf();

	glFramebufferTexture(GLTarget(), attachment, cube_map_buffer->GetID(), mipmap_level);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	cube_map_buffer->UnbindSelf();
	UnbindSelf();
}

void FrameBuffer::AddRenderBuffer()
{
	// 创建一个渲染缓冲对象
	glGenRenderbuffers(1, &RBO);
	// 绑定渲染缓冲对象rbo到GL_RENDERBUFFER，让之后所有的渲染缓冲操作影响当前的rbo
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	// 给渲染缓冲对象分配内存。
	// GL_DEPTH24_STENCIL8作为内部格式，它封装了24位的深度和8位的模板缓冲，将该渲染缓冲对象用于深度和模板渲染缓冲
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, buffer_width, buffer_height);
	// 解绑
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// 将渲染缓冲对象附加到帧缓冲上
	glFramebufferRenderbuffer(GLTarget(), GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	
	CheckStatus();

	// 解绑帧缓冲
	UnbindSelf();
}

void FrameBuffer::CheckStatus()
{
	if (glCheckFramebufferStatus(GLTarget()) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
}
