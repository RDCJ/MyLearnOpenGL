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
