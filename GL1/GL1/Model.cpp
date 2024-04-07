#include "Model.h"

void Model::Draw(ShaderProgram& shader)
{
	for (Mesh& mesh : meshes)
		mesh.Draw(shader);
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;

	// ReadFile�ĵڶ���������һЩ���ڴ���(Post-processing)��ѡ��: https://assimp.sourceforge.net/lib_html/postprocess_8h.html
	// aiProcess_Triangulate: ���ģ�Ͳ��ǣ�ȫ��������������ɣ�����Ҫ��ģ�����е�ͼԪ��״�任Ϊ�����Ρ�
	// aiProcess_FlipUVs: ���ڴ����ʱ��תy�����������
	// aiProcess_GenNormals�����ģ�Ͳ������������Ļ�����Ϊÿ�����㴴�����ߡ�
	// aiProcess_SplitLargeMeshes�����Ƚϴ������ָ�ɸ�С����������������Ⱦ����󶥵������ƣ�ֻ����Ⱦ��С��������ô����ǳ�����
	// aiProcess_OptimizeMeshes�������С����ƴ��Ϊһ��������񣬼��ٻ��Ƶ��ôӶ������Ż�
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// ������ģ��֮�󣬼�鳡��������ڵ㲻Ϊnull�����Ҽ��������һ�����(Flag)�����鿴���ص������ǲ��ǲ�������
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}

	directory = path.substr(0, path.find_last_of("/"));
	this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	// ����˽ڵ����е�mesh������еĻ���
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Scene�µ�mMeshes���鴢����������Mesh���󣬽ڵ��е�mMeshes���鱣���ֻ�ǳ��������������������
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	// �������������ӽڵ��ظ���һ����
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	// ��ȡ��������
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		v.Position = Utils::ToGlmV3(mesh->mVertices[i]);
		v.Normal = Utils::ToGlmV3(mesh->mNormals[i]);

		// Assimp����һ��ģ����һ�������������8����ͬ���������꣬����ֻ���ĵ�һ����������
		if (mesh->mTextureCoords[0]) // ��������Ƿ���İ�������������
		{
			v.TexCoords = Utils::ToGlmV2(mesh->mTextureCoords[0][i]);
		}
		else
		{
			v.TexCoords = glm::vec2(0);
		}
	}

	// ��ȡ����
	// Assimp�Ľӿڶ�����ÿ��������һ����(Face)���飬ÿ���������һ��ͼԪ��
	// ����趨��aiProcess_Triangulate�� ÿ������������
	// һ��������˶�����������Ƕ�������ÿ��ͼԪ�У�Ӧ�û����ĸ����㣬����ʲô˳�����
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// ��ȡ����
	// һ������ֻ������һ��ָ����ʶ���������������Ҫ��ȡ���������Ĳ��ʣ����ǻ���Ҫ����������mMaterials����
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
	// GetTextureCount������鴢���ڲ��������������
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		// GetTexture��ȡÿ��������ļ�λ��, ���Ὣ���������һ��aiString��
		material->GetTexture(type, i, &str);
		// ������ģ���ļ��������ļ���·���������ģ���ļ��ı���(Local)·��
		Texture2D texture = Texture2D::GetTexture2D(str.C_Str(), this->directory, typeName);

		textures.push_back(texture);
	}
	return textures;
}
