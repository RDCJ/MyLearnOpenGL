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
	// 绑定纹理
	std::map<std::string, unsigned int> texture_count;
	for (std::string texture_type : Texture2D::TextureTypes) texture_count[texture_type] = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		std::string &type = textures[i].type;
		unsigned int idx = texture_count[type];
		// 纹理命名规则：material.texture_{type}{idx}
		shader.SetUniformInt("material." + type + std::to_string(idx), i);

		texture_count[type] = idx + 1;

		// 在绑定之前激活相应的纹理单元
		glActiveTexture(GL_TEXTURE + i);
		textures[i].Bind();
	}

	// 绘制mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);

	// glGenBuffers函数用来生成缓冲区对象的名称, 第一个参数是要生成的缓冲对象的数量，第二个参数是用来存储缓冲对象名称的数组
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
	// C++结构体的内存布局是连续的(Sequential)。
	// 也就是说，如果我们将结构体作为一个数据数组使用，那么它将会以顺序排列结构体的变量，这将会直接转换为我们在数组缓冲中所需要的float（实际上是字节）数组
	// 我们能够直接传入一大列的Vertex结构体的指针作为缓冲的数据，它们将会完美地转换为glBufferData所能用的参数
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// offsetof(s, m)，它的第一个参数是一个结构体，第二个参数是这个结构体中变量的名字。这个宏会返回那个变量距结构体头部的字节偏移量

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glBindVertexArray(0);
}
