#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int buffer_width, int buffer_height): buffer_width(buffer_width), buffer_height(buffer_height)
{
	// ����һ��֡�������
	Generate();
}

void FrameBuffer::AddTexture2D(GLenum format, GLenum data_type, int mipmap_level, const TexParams& params)
{
	// ���Ǹ������data����������NULL, ��������������ǽ����������ڴ��û�����������������������������Ⱦ��֡����֮��������
	color_buffer = new Texture2D(buffer_width, buffer_height, format, NULL, data_type);

	color_buffer->SetParameters(params);

	GLenum attachment;
	if (format == GL_DEPTH_COMPONENT)
		attachment = GL_DEPTH_ATTACHMENT;
	else
		attachment = GL_COLOR_ATTACHMENT0;

	BindSelf();
	// �������ӵ�֡�����ϣ� ֮�����е���Ⱦָ���д�뵽���������
	// glFramebufferTexture2D������
	// target��֡�����Ŀ�꣨���ơ���ȡ�������߽��У�
	// attachment��������Ҫ���ӵĸ������͡���ǰ�������ڸ���һ����ɫ������ע������0��ζ�����ǿ��Ը��Ӷ����ɫ���������ǽ���֮��Ľ̳����ᵽ��
	//	textarget����ϣ�����ӵ���������
	// texture��Ҫ���ӵ�����id
	//	level��mipmap�ļ���
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
	// ����һ����Ⱦ�������
	glGenRenderbuffers(1, &RBO);
	// ����Ⱦ�������rbo��GL_RENDERBUFFER����֮�����е���Ⱦ�������Ӱ�쵱ǰ��rbo
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	// ����Ⱦ�����������ڴ档
	// GL_DEPTH24_STENCIL8��Ϊ�ڲ���ʽ������װ��24λ����Ⱥ�8λ��ģ�建�壬������Ⱦ�������������Ⱥ�ģ����Ⱦ����
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, buffer_width, buffer_height);
	// ���
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// ����Ⱦ������󸽼ӵ�֡������
	glFramebufferRenderbuffer(GLTarget(), GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	
	CheckStatus();

	// ���֡����
	UnbindSelf();
}

void FrameBuffer::CheckStatus()
{
	if (glCheckFramebufferStatus(GLTarget()) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
}
