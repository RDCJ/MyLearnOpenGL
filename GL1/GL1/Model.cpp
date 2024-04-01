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
	// glBufferData�������û���������ݸ��Ƶ���ǰ�󶨻���
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void Model::BufferIndices()
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Model::Draw()
{
	// glDrawElements��һ������ָ���˻��Ƶ�ģʽ, �ڶ��������ǻ��ƶ���ĸ������������������������������͡����һ������ָ��EBO�е�ƫ����
// glDrawElements�����ӵ�ǰ�󶨵�GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ������
// �ڰ�VAOʱ���󶨵����һ��Ԫ�ػ���������洢ΪVAO��Ԫ�ػ���������Ȼ�󣬰󶨵�VAOҲ���Զ��󶨸�EBO
	glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);
}