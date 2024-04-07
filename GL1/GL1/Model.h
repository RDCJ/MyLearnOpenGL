#pragma once

#ifndef MODEL_H
#include <vector>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture2D.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Utils.h"

/// <summary>
/// ʹ��Assimp������ģ�ͣ�������ת��(Translate)�����Mesh����
/// ���еĳ���/ģ�����ݶ�������Scene�����С�Scene����Ҳ�����˳������ڵ�����á�
/// ������Root node�����ڵ㣩���ܰ����ӽڵ㣨�������Ľڵ�һ������������һϵ��ָ�򳡾�������mMeshes�����д�����������ݵ�������
/// </summary>
class Model
{
	public:
		Model(const char* path)
		{
			LoadModel(path);
		}

		/// <summary>
		/// ������mesh�������Ǹ��Ե�Draw����
		/// </summary>
		/// <param name="shader"></param>
		void Draw(ShaderProgram& shader);



	private:
		std::vector<Mesh> meshes;
		/// <summary>
		/// ģ���ļ�·��
		/// </summary>
		std::string directory;

		/// <summary>
		/// ʹ��Assimp������ģ����Assimp��һ������Scene�����ݽṹ��
		/// </summary>
		/// <param name="path"></param>
		void LoadModel(std::string path);
		/// <summary>
		/// �������е����нڵ�, ��ȡ��Ӧ��meshes
		/// </summary>
		/// <param name="node"></param>
		/// <param name="scene"></param>
		void ProcessNode(aiNode* node, const aiScene* scene);
		/// <summary>
		/// ��һ��aiMesh����ת��Ϊ�Զ����Mesh��������ô����
		/// </summary>
		/// <param name="mesh"></param>
		/// <param name="scene"></param>
		/// <returns></returns>
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture2D> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};
#endif // !MODEL_H



