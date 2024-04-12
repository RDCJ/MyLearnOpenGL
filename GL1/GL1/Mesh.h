#pragma once

#ifndef MESH_H
#include <glm/glm.hpp>
#include <vector>

#include "ShaderProgram.h"
#include "IDrawable.h"
#include "TextureCubeMap.h"

class Vertex
{
	public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
};

class Mesh : public IDrawable
{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);

		void Draw(ShaderProgram& shader) override;
		void DrawInstance(ShaderProgram& shader, int num);

	private:
		/// <summary>
		/// EBO是一个缓冲区，存储 OpenGL 用来决定要绘制哪些顶点的索引
		/// </summary>
		unsigned int EBO;
		unsigned int VAO, VBO;

		void SetupMesh();
};
#endif // !MESH_H



