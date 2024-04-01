#include "Model.h"

Model::Model(int _vertex_count, const float* _vertices, int _vertices_size, const unsigned int* _indices, int _indices_size)
{
	SetVertices(_vertex_count, _vertices, _vertices_size);
	SetIndices(_indices, _indices_size);
}

void Model::SetVertices(int _vertex_count, const float* _vertices, int size)
{
	vertices = std::vector<float>(_vertices, _vertices + size);
	vertex_count = _vertex_count;
}

void Model::SetIndices(const unsigned int* _indices, int size)
{
	indices = std::vector<unsigned int>(_indices, _indices + size);

	triangle_count = size / 3;
}

void Model::BufferVertices()
{
	// glBufferData用来把用户定义的数据复制到当前绑定缓冲
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void Model::BufferIndices()
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Model::Draw()
{
	// glDrawElements第一个参数指定了绘制的模式, 第二个参数是绘制顶点的个数。第三个参数是索引的数据类型。最后一个参数指定EBO中的偏移量
// glDrawElements函数从当前绑定到GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取其索引
// 在绑定VAO时，绑定的最后一个元素缓冲区对象存储为VAO的元素缓冲区对象。然后，绑定到VAO也会自动绑定该EBO
	glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);
}