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
		// 将每个顶点切线/副切线平均化，以获得更加柔和的效果
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

	// 顶点属性最大允许的数据大小等于一个vec4。
	// 因为一个mat4本质上是4个vec4，因此需要为这个矩阵预留4个顶点属性
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(location + i);
		glVertexAttribPointer(location + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(i * vec4_size));

		// glVertexAttribDivisor告诉了OpenGL该什么时候更新顶点属性的内容至新一组数据
		// 第一个参数指定顶点属性，第二个参数是属性除数
		// 默认情况下，属性除数是0，顶点着色器每次迭代时更新顶点属性
		// 将它设置为1时，OpenGL在渲染一个新实例的时候更新顶点属性
		// 设置为2时，每2个实例更新一次属性，以此类推
		glVertexAttribDivisor(location + i, 1);
	}
	glBindVertexArray(0);
}

void Mesh::Draw(ShaderProgram& shader)
{
	//// 先解除其他纹理的绑定
	//for (int i = 0; i < 15; i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
	// 绘制mesh
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

void Mesh::SetupMeshBatchedVertex()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

#pragma region 配置VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 调用glBufferSubData之前必须要先调用glBufferData, 预留特定大小的内存
	size_t size_need = Utils::MemorySize(Position) + Utils::MemorySize(Normal) +
								  Utils::MemorySize(TexCoords) + Utils::MemorySize(Tangent) + Utils::MemorySize(Bitangent);
	glBufferData(GL_ARRAY_BUFFER, size_need, NULL, GL_STATIC_DRAW);
	// 传输顶点数据
	int offset = 0;
	BufferSubData(Position, 0, offset);
	BufferSubData(Normal, 1, offset);
	BufferSubData(TexCoords, 2, offset);
	BufferSubData(Tangent, 3, offset);
	BufferSubData(Bitangent, 4, offset);
#pragma endregion

#pragma region 配置EBO
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
