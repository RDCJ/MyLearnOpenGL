#pragma once

#ifndef MESH_H
#include <glm/glm.hpp>
#include <vector>

#include "Texture2D.h"
#include "ShaderProgram.h"

class Vertex
{
	public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 TexCoords;
};

class Mesh
{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture2D> textures;

		Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture2D> _textures);

		void Draw(ShaderProgram& shader);

	private:
		unsigned int VAO, VBO, EBO;

		void SetupMesh();
};
#endif // !MESH_H



