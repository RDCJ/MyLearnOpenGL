#include "UniformBuffer.h"


void UniformBuffer::GenBuffer(int size)
{
	// 创建Uniform缓冲对象
	if (ID == 0) glGenBuffers(1, &ID);
	// 分配内存
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);

	// glBindbufferBase将Uniform缓冲对象绑定到绑定点上
	// 参数：目标，绑定点，Uniform缓冲对象
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ID);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::GenBuffer(int size, int offset)
{
	// 创建Uniform缓冲对象
	if (ID == 0) glGenBuffers(1, &ID);
	// 分配内存
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);

	// glBindBufferRange绑定Uniform缓冲的特定一部分到绑定点中
	// 参数：目标，绑定点，Uniform缓冲对象, 偏移量, 大小
	glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, ID, offset, size);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
