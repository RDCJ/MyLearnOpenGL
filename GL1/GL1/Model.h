#pragma once
#ifndef MODEL_H
#include <vector>
#include <glad/glad.h>

class Model
{
	public:
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		int vertex_count;
		int triangle_count;

		Model(int _vertex_count, const float* _vertices, int _vertices_size, const unsigned int* _indices, int _indices_size);

		void SetVertices(int _vertex_count, const float* _vertices, int size);

		void SetIndices(const unsigned int* _indices, int size);
		
		/// <summary>
		/// �Ѷ������ݸ��Ƶ�����
		/// </summary>
		void BufferVertices();
		/// <summary>
		/// ���������Ƶ�����
		/// </summary>
		void BufferIndices();

		void Draw();

	private:
};

#endif // !MODEL_H


