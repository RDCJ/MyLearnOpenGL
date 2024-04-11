#include "UniformBuffer.h"


void UniformBuffer::GenBuffer(int size)
{
	// ����Uniform�������
	if (ID == 0) glGenBuffers(1, &ID);
	// �����ڴ�
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);

	// glBindbufferBase��Uniform�������󶨵��󶨵���
	// ������Ŀ�꣬�󶨵㣬Uniform�������
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ID);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::GenBuffer(int size, int offset)
{
	// ����Uniform�������
	if (ID == 0) glGenBuffers(1, &ID);
	// �����ڴ�
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);

	// glBindBufferRange��Uniform������ض�һ���ֵ��󶨵���
	// ������Ŀ�꣬�󶨵㣬Uniform�������, ƫ����, ��С
	glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, ID, offset, size);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
