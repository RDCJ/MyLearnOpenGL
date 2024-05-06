#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Utils.h"

/// <summary>
/// 使用Assimp来加载模型，并将它转换(Translate)至多个Mesh对象
/// 所有的场景/模型数据都包含在Scene对象中。Scene对象也包含了场景根节点的引用。
/// 场景的Root node（根节点）可能包含子节点（和其它的节点一样），它会有一系列指向场景对象中mMeshes数组中储存的网格数据的索引。
/// </summary>
class Model: IDrawable
{
	public:
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
		Model(const char* path, bool texture_flip_vertical=false):texture_flip_vertical(texture_flip_vertical)
		{
			LoadModel(path);
		}

		/// <summary>
		/// 对所有mesh调用它们各自的Draw函数
		/// </summary>
		/// <param name="shader"></param>
		void Draw(ShaderProgram& shader) override;
		/// <summary>
		/// 对所有mesh调用它们各自的DrawInstance函数
		/// </summary>
		/// <param name="shader"></param>
		/// <param name="num"></param>
		void DrawInstance(ShaderProgram& shader, int num) override;

	private:
		/// <summary>
		/// 模型文件路径
		/// </summary>
		std::string directory;

		/// <summary>
		/// 使用Assimp来加载模型至Assimp的一个叫做Scene的数据结构中
		/// </summary>
		/// <param name="path"></param>
		void LoadModel(std::string path);
		/// <summary>
		/// 处理场景中的所有节点, 获取对应的meshes
		/// </summary>
		/// <param name="node"></param>
		/// <param name="scene"></param>
		void ProcessNode(aiNode* node, const aiScene* scene);
		/// <summary>
		/// 将一个aiMesh对象转化为自定义的Mesh对象不是那么困难
		/// </summary>
		/// <param name="mesh"></param>
		/// <param name="scene"></param>
		/// <returns></returns>
		std::tuple<Mesh, Material> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture2D> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

		bool texture_flip_vertical;

};



