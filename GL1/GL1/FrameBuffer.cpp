#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int buffer_width, int buffer_height): buffer_width(buffer_width), buffer_height(buffer_height)
{
	// ����һ��֡�������
	glGenFramebuffers(1, &ID);
}

void FrameBuffer::AddTexture(GLenum format, int mipmap_level)
{
	Bind();
	// ���Ǹ������data����������NULL, ��������������ǽ����������ڴ��û�����������������������������Ⱦ��֡����֮��������
	color_buffer = new Texture2D(buffer_width, buffer_height, format, NULL);

	std::map<GLenum, GLenum> params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST},
		// ���ڶ���Ļ����ı�Ե���в�����ʱ�����ڻ��������������Χ��8�����ؽ��в�������ʵ��ȡ������֮������ء�
		// ���ڻ��Ʒ�ʽĬ����GL_REPEAT��������û�����õ������ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬��Ϳ��ܻ�����Ļ��Ե��������ֵ����ơ�
		// Ϊ��������һ���⣬���ǿ��Խ���Ļ����Ļ��Ʒ�ʽ������ΪGL_CLAMP_TO_EDGE��
		// ��������ȡ�������������ʱ�����ܹ��ظ���Ե������������ȷ�ع������յ�ֵ�ˡ�
		{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
		{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},
	};
	color_buffer->SetParameters(params);

	// �������ӵ�֡�����ϣ� ֮�����е���Ⱦָ���д�뵽���������
	// glFramebufferTexture2D������
	// target��֡�����Ŀ�꣨���ơ���ȡ�������߽��У�
	// attachment��������Ҫ���ӵĸ������͡���ǰ�������ڸ���һ����ɫ������ע������0��ζ�����ǿ��Ը��Ӷ����ɫ���������ǽ���֮��Ľ̳����ᵽ��
	//	textarget����ϣ�����ӵ���������
	// texture��Ҫ���ӵ�����id
	//	level��mipmap�ļ���
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer->GetID(), mipmap_level);
	glBindTexture(GL_TEXTURE_2D, 0);
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
