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
		/// 把顶点数据复制到缓冲
		/// </summary>
		void BufferVertices();
		/// <summary>
		/// 把索引复制到缓冲
		/// </summary>
		void BufferIndices();

		void Draw();

	private:
};

#endif // !MODEL_H


