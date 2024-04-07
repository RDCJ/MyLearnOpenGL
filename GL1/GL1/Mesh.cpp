#include "Mesh.h"
#include <map>

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture2D> _textures)
{
	vertices = _vertices;
	indices = _indices;
	textures = _textures;

	SetupMesh();
}

void Mesh::Draw(ShaderProgram& shader)
{
	// ������
	std::map<std::string, unsigned int> texture_count;
	for (std::string texture_type : Texture2D::TextureTypes) texture_count[texture_type] = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		std::string &type = textures[i].type;
		unsigned int idx = texture_count[type];
		// ������������material.texture_{type}{idx}
		shader.SetUniformInt("material." + type + std::to_string(idx), i);

		texture_count[type] = idx + 1;

		// �ڰ�֮ǰ������Ӧ������Ԫ
		glActiveTexture(GL_TEXTURE + i);
		textures[i].Bind();
	}

	// ����mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
