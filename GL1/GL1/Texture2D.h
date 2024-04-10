#pragma once
#ifndef  TEXTURE2D_H

#include <glad/glad.h>
#include "stb_image.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

class Texture2D
{
	public:
		unsigned int GetID() { return ID; }
		Texture2D() { type = ""; ID = 0; }
		Texture2D(const char* image_path, std::string _type, bool flip_vertical=true);
		/// <summary>
		/// 返回全局的纹理储存中的目标纹理，如果尚未加载就先从文件加载到全局储存中再返回
		/// </summary>
		/// <param name="file_name"></param>
		/// <param name="model_directory"></param>
		/// <param name="_type"></param>
		/// <returns></returns>
		static Texture2D GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type, bool flip_vertical=true);
		void SetParameters();
		void GenerateMipmap();
		/// <summary>
		/// 绑定到GL_TEXTURE_2D
		/// </summary>
		void Bind();
		/// <summary>
		/// 纹理的类型
		/// </summary>
		std::string type;
		/// <summary>
		/// 所有的纹理类型
		/// </summary>
		static const std::vector<std::string> TextureTypes;


	private:
		static std::unordered_map<std::string, Texture2D> LoadedTextures;
		unsigned int ID = 0;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data);
		
};

#endif // ! TEXTURE2D_H



