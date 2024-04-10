#include "Mesh.h"
#include <map>

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture2D> _textures, TextureCubeMap* _cube_map)
{
	vertices = _vertices;
	indices = _indices;
	textures = _textures;
	cube_map = _cube_map;

	SetupMesh();
}

void Mesh::Draw(ShaderProgram& shader)
{
	// 先解除其他纹理的绑定
	for (int i = 0; i < 10; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

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
		glActiveTexture(GL_TEXTURE0 + i);
		textures[i].Bind();
	}

	shader.SetUniformBool("use_cube_map", cube_map != nullptr);
	if (cube_map != nullptr)
	{
		shader.SetUniformInt("cube_map", textures.size());
		glActiveTexture(GL_TEXTURE0 + textures.size());
		cube_map->Bind();
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

	// glEnableVertexAttribArray以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的
	glEnableVertexAttribArray(0);
	/*
		glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据
		第一个参数指定我们要配置的顶点属性, 对应顶点着色器中的 layout (location == ?)
		第二个参数指定顶点属性的大小
		第三个参数指定数据的类型
		第四个参数定义我们是否希望数据被标准化。如果设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间
		第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔，简单说就是从这个属性第二次出现的位置到第一次出现的位置之间有多少字节
		最后一个参数表示位置数据在缓冲中起始位置的偏移量，类型是void*，所以需要进行强制类型转换

		每个顶点属性从一个VBO管理的内存中获得它的数据，而具体是从哪个VBO（程序中可以有多个VBO）获取则是通过在调用glVertexAttribPointer时绑定到GL_ARRAY_BUFFER的VBO决定的
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}
