#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int buffer_width, int buffer_height): buffer_width(buffer_width), buffer_height(buffer_height)
{
	// ����һ��֡�������
	glGenFramebuffers(1, &ID);
}

void FrameBuffer::AddTexture(GLenum format, GLenum data_type, int mipmap_level, const TexParams& params)
{
	// ���Ǹ������data����������NULL, ��������������ǽ����������ڴ��û�����������������������������Ⱦ��֡����֮��������
	color_buffer = new Texture2D(buffer_width, buffer_height, format, NULL, data_type);

	color_buffer->SetParameters(params);

	GLenum attachment;
	if (format == GL_DEPTH_COMPONENT)
		attachment = GL_DEPTH_ATTACHMENT;
	else
		attachment = GL_COLOR_ATTACHMENT0;

	Bind();
	// �������ӵ�֡�����ϣ� ֮�����е���Ⱦָ���д�뵽���������
	// glFramebufferTexture2D������
	// target��֡�����Ŀ�꣨���ơ���ȡ�������߽��У�
	// attachment��������Ҫ���ӵĸ������͡���ǰ�������ڸ���һ����ɫ������ע������0��ζ�����ǿ��Ը��Ӷ����ɫ���������ǽ���֮��Ľ̳����ᵽ��
	//	textarget����ϣ�����ӵ���������
	// texture��Ҫ���ӵ�����id
	//	level��mipmap�ļ���
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, color_buffer->GetID(), mipmap_level);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	// ���֡�����Ƿ��������ģ�������ǣ�����ӡ������Ϣ��
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// ���֡����
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind()
{
	// glBindFramebuffer: ��֡����, �󶨵�GL_FRAMEBUFFERĿ��֮�����еĶ�ȡ��д��֡����Ĳ�������Ӱ�쵱ǰ�󶨵�֡����
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}
