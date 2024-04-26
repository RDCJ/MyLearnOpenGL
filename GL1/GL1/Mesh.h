#pragma once

#ifndef MESH_H
#include <glm/glm.hpp>
#include <vector>

#include "ShaderProgram.h"
#include "IDrawable.h"
#include "TextureCubeMap.h"
#include "Utils.h"

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

#pragma region ������������, layout���ն���˳���Ӧ0~5
		std::vector<glm::vec3> Position;
		std::vector<glm::vec3> Normal;
		std::vector<glm::vec2> TexCoords;
		std::vector<glm::vec3> Tangent;
		std::vector<glm::vec3> Bitangent;
#pragma endregion

		std::vector<unsigned int> indices;

		/// <summary>
		/// �������ݽ�����
		/// </summary>
		/// <param name="_vertices"></param>
		/// <param name="_indices"></param>
		Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);
		/// <summary>
		/// �������ݷ�������
		/// </summary>
		/// <param name="Position"></param>
		/// <param name="_indices"></param>
		/// <param name="Normal">optional</param>
		/// <param name="TexCoords">optional</param>
		/// <param name="Tangent">optional</param>
		/// <param name="Bitangent">optional</param>
		Mesh(std::vector<glm::vec3>& Position, std::vector<unsigned int>& _indices,
			std::vector<glm::vec3>* Normal = nullptr, std::vector<glm::vec2>* TexCoords = nullptr,
			std::vector<glm::vec3>* Tangent = nullptr, std::vector<glm::vec3>* Bitangent = nullptr);
		/// <summary>
		/// �������ݷ�������
		/// </summary>
		/// <param name="Position"></param>
		/// <param name="_indices"></param>
		/// <param name="Normal">optional</param>
		/// <param name="TexCoords">optional</param>
		/// <param name="calc_TBN">�Ƿ�����TBN</param>
		Mesh(std::vector<glm::vec3>& Position, std::vector<unsigned int>& _indices,
			std::vector<glm::vec3>* Normal = nullptr, std::vector<glm::vec2>* TexCoords = nullptr,
			bool calc_TBN=false);

		void SetInstanceMat4(int location);

		void Draw(ShaderProgram& shader) override;
		void DrawInstance(ShaderProgram& shader, int num) override;

	private:
		/// <summary>
		/// EBO��һ�����������洢 OpenGL ��������Ҫ������Щ���������
		/// </summary>
		unsigned int EBO = 0;
		unsigned int VAO = 0, VBO = 0;

		void SetupMesh();

		void SetupMeshBatchedVertex();

		/// <summary>
		/// ��һ��glm::vec3���͵�������������
		/// </summary>
		/// <param name="vertex_data"></param>
		/// <param name="layout"></param>
		/// <param name="offset"></param>
		static void BufferSubData(std::vector<glm::vec3>& vertex_data, int layout, int& offset);

		/// <summary>
		/// ��һ��glm::vec2���͵�������������
		/// </summary>
		/// <param name="vertex_data"></param>
		/// <param name="layout"></param>
		/// <param name="offset"></param>
		static void BufferSubData(std::vector<glm::vec2>& vertex_data, int layout, int& offset);

};
#endif // !MESH_H



