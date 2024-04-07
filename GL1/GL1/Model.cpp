#include "Model.h"

void Model::Draw(ShaderProgram& shader)
{
	for (Mesh& mesh : meshes)
		mesh.Draw(shader);
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;

	// ReadFile的第二个参数是一些后期处理(Post-processing)的选项: https://assimp.sourceforge.net/lib_html/postprocess_8h.html
	// aiProcess_Triangulate: 如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
	// aiProcess_FlipUVs: 将在处理的时候翻转y轴的纹理坐标
	// aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
	// aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用
	// aiProcess_OptimizeMeshes：将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// 加载了模型之后，检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}

	directory = path.substr(0, path.find_last_of("/"));
	this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	// 处理此节点所有的mesh（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Scene下的mMeshes数组储存了真正的Mesh对象，节点中的mMeshes数组保存的只是场景中网格数组的索引。
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	// 提取顶点数据
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		v.Position = Utils::ToGlmV3(mesh->mVertices[i]);
		v.Normal = Utils::ToGlmV3(mesh->mNormals[i]);

		// Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标，我们只关心第一组纹理坐标
		if (mesh->mTextureCoords[0]) // 检查网格是否真的包含了纹理坐标
		{
			v.TexCoords = Utils::ToGlmV2(mesh->mTextureCoords[0][i]);
		}
		else
		{
			v.TexCoords = glm::vec2(0);
		}
	}

	// 提取索引
	// Assimp的接口定义了每个网格都有一个面(Face)数组，每个面代表了一个图元，
	// 如果设定了aiProcess_Triangulate， 每个面是三角形
	// 一个面包含了多个索引，它们定义了在每个图元中，应该绘制哪个顶点，并以什么顺序绘制
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 提取材质
	// 一个网格只包含了一个指向材质对象的索引。如果想要获取网格真正的材质，我们还需要索引场景的mMaterials数组
	if (mesh->mMaterialIndex > 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture2D> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		std::vector<Texture2D> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture2D> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture2D> textures;
	// GetTextureCount函数检查储存在材质中纹理的数量
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		// GetTexture获取每个纹理的文件位置, 它会将结果储存在一个aiString中
		material->GetTexture(type, i, &str);
		// 假设了模型文件中纹理文件的路径是相对于模型文件的本地(Local)路径
		Texture2D texture = Texture2D::GetTexture2D(str.C_Str(), this->directory, typeName);

		textures.push_back(texture);
	}
	return textures;
}
