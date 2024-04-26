#include "Mesh.h"
#include <map>


Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices)
{
	vertices = _vertices;
	indices = _indices;

	SetupMesh();
}

Mesh::Mesh(std::vector<glm::vec3>& Position, std::vector<unsigned int>& _indices, std::vector<glm::vec3>* Normal, 
					std::vector<glm::vec2>* TexCoords, std::vector<glm::vec3>* Tangent, std::vector<glm::vec3>* Bitangent)
{
	this->Position = Position;
	this->indices = _indices;
	if (Normal != nullptr) this->Normal = *Normal;
	if (TexCoords != nullptr) this->TexCoords = *TexCoords;
	if (Tangent != nullptr) this->Tangent = *Tangent;
	if (Bitangent != nullptr) this->Bitangent = *Bitangent;

	SetupMeshBatchedVertex();
}

Mesh::Mesh(std::vector<glm::vec3>& Position, std::vector<unsigned int>& _indices,
					std::vector<glm::vec3>* Normal, std::vector<glm::vec2>* TexCoords, bool calc_TBN)
{
	this->Position = Position;
	this->indices = _indices;
	if (Normal != nullptr) this->Normal = *Normal;
	if (TexCoords != nullptr) this->TexCoords = *TexCoords;

	if (calc_TBN)
	{
		int vertex_amount = this->Position.size();
		int triangle_amount = this->indices.size() / 3;

		this->Tangent = std::vector<glm::vec3>(vertex_amount, glm::vec3(0));
		this->Bitangent = std::vector<glm::vec3>(vertex_amount, glm::vec3(0));
		std::vector<unsigned int> vertex_count(vertex_amount, 0);
		
		for (int i = 0; i < triangle_amount; i++)
		{
			int idx1 = this->indices[i * 3], idx2 = this->indices[i * 3 + 1], idx3 = this->indices[i * 3 + 2];

			auto TB = Utils::CalcTBN(this->Position[idx1], this->Position[idx2], this->Position[idx3],
				this->TexCoords[idx1], this->TexCoords[idx2], this->TexCoords[idx3]);
			this->Tangent[idx1] += std::get<0>(TB);
			this->Bitangent[idx1] += std::get<1>(TB);

			TB = Utils::CalcTBN(this->Position[idx2], this->Position[idx1], this->Position[idx3],
				this->TexCoords[idx2], this->TexCoords[idx1], this->TexCoords[idx3]);
			this->Tangent[idx2] += std::get<0>(TB);
			this->Bitangent[idx2] += std::get<1>(TB);

			TB = Utils::CalcTBN(this->Position[idx3], this->Position[idx2], this->Position[idx1],
				this->TexCoords[idx3], this->TexCoords[idx2], this->TexCoords[idx1]);
			this->Tangent[idx3] += std::get<0>(TB);
			this->Bitangent[idx3] += std::get<1>(TB);

			vertex_count[idx1]++;
			vertex_count[idx2]++;
			vertex_count[idx3]++;
		}
		// ��ÿ����������/������ƽ�������Ի�ø�����͵�Ч��
		for (int i = 0; i < vertex_amount; i++)
		{
			this->Tangent[i] /= vertex_count[i];
			this->Bitangent[i] /= vertex_count[i];
		}
	}

	SetupMeshBatchedVertex();
}

void Mesh::SetInstanceMat4(int location)
{
	glBindVertexArray(VAO);
	GLsizei vec4_size = sizeof(glm::vec4);

	// �������������������ݴ�С����һ��vec4��
	// ��Ϊһ��mat4��������4��vec4�������ҪΪ�������Ԥ��4����������
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(location + i);
		glVertexAttribPointer(location + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(i * vec4_size));

		// glVertexAttribDivisor������OpenGL��ʲôʱ����¶������Ե���������һ������
		// ��һ������ָ���������ԣ��ڶ������������Գ���
		// Ĭ������£����Գ�����0��������ɫ��ÿ�ε���ʱ���¶�������
		// ��������Ϊ1ʱ��OpenGL����Ⱦһ����ʵ����ʱ����¶�������
		// ����Ϊ2ʱ��ÿ2��ʵ������һ�����ԣ��Դ�����
		glVertexAttribDivisor(location + i, 1);
	}
	glBindVertexArray(0);
}

void Mesh::Draw(ShaderProgram& shader)
{
	//// �Ƚ����������İ�
	//for (int i = 0; i < 15; i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
	// ����mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::DrawInstance(ShaderProgram& shader, int num)
{
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, num);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);

	// glGenBuffers�����������ɻ��������������, ��һ��������Ҫ���ɵĻ��������������ڶ��������������洢����������Ƶ�����
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
	// C++�ṹ����ڴ沼����������(Sequential)��
	// Ҳ����˵��������ǽ��ṹ����Ϊһ����������ʹ�ã���ô��������˳�����нṹ��ı������⽫��ֱ��ת��Ϊ���������黺��������Ҫ��float��ʵ�������ֽڣ�����
	// �����ܹ�ֱ�Ӵ���һ���е�Vertex�ṹ���ָ����Ϊ��������ݣ����ǽ���������ת��ΪglBufferData�����õĲ���
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// offsetof(s, m)�����ĵ�һ��������һ���ṹ�壬�ڶ�������������ṹ���б��������֡������᷵���Ǹ�������ṹ��ͷ�����ֽ�ƫ����

	// glEnableVertexAttribArray�Զ�������λ��ֵ��Ϊ���������ö������ԣ���������Ĭ���ǽ��õ�
	glEnableVertexAttribArray(0);
	/*
		glVertexAttribPointer��������OpenGL����ν�����������
		��һ������ָ������Ҫ���õĶ�������, ��Ӧ������ɫ���е� layout (location == ?)
		�ڶ�������ָ���������ԵĴ�С
		����������ָ�����ݵ�����
		���ĸ��������������Ƿ�ϣ�����ݱ���׼�����������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮��
		�����������������(Stride)�������������������Ķ���������֮��ļ������˵���Ǵ�������Եڶ��γ��ֵ�λ�õ���һ�γ��ֵ�λ��֮���ж����ֽ�
		���һ��������ʾλ�������ڻ�������ʼλ�õ�ƫ������������void*��������Ҫ����ǿ������ת��

		ÿ���������Դ�һ��VBO������ڴ��л���������ݣ��������Ǵ��ĸ�VBO�������п����ж��VBO����ȡ����ͨ���ڵ���glVertexAttribPointerʱ�󶨵�GL_ARRAY_BUFFER��VBO������
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::SetupMeshBatchedVertex()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

#pragma region ����VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// ����glBufferSubData֮ǰ����Ҫ�ȵ���glBufferData, Ԥ���ض���С���ڴ�
	size_t size_need = Utils::MemorySize(Position) + Utils::MemorySize(Normal) +
								  Utils::MemorySize(TexCoords) + Utils::MemorySize(Tangent) + Utils::MemorySize(Bitangent);
	glBufferData(GL_ARRAY_BUFFER, size_need, NULL, GL_STATIC_DRAW);
	// ���䶥������
	int offset = 0;
	BufferSubData(Position, 0, offset);
	BufferSubData(Normal, 1, offset);
	BufferSubData(TexCoords, 2, offset);
	BufferSubData(Tangent, 3, offset);
	BufferSubData(Bitangent, 4, offset);
#pragma endregion

#pragma region ����EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
#pragma endregion

	glBindVertexArray(0);
}

inline void Mesh::BufferSubData(std::vector<glm::vec3>& vertex_data, int layout, int& offset)
{
	if (vertex_data.size() > 0)
	{
		const int vec_len = 3;
		const size_t vec_size = sizeof(*vertex_data.data());
		const size_t data_size = vertex_data.size() * vec_size;
		glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, vertex_data.data());
		glEnableVertexAttribArray(layout);
		glVertexAttribPointer(layout, vec_len, GL_FLOAT, GL_FALSE, vec_size, (void*)offset);
		offset += data_size;
	}
}

inline void Mesh::BufferSubData(std::vector<glm::vec2>& vertex_data, int layout, int& offset)
{
	if (vertex_data.size() > 0)
	{
		const int vec_len = 2;
		const size_t vec_size = sizeof(*vertex_data.data());
		const size_t data_size = vertex_data.size() * vec_size;
		glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, vertex_data.data());
		glEnableVertexAttribArray(layout);
		glVertexAttribPointer(layout, vec_len, GL_FLOAT, GL_FALSE, vec_size, (void*)offset);
		offset += data_size;
	}
}
