#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
#include "Texture.h"

class Texture2D: public Texture
{
	public:
		static const TexParams DefaultParams;
		Texture2D() { type = "";}

		Texture2D(const char* image_path, std::string _type, bool flip_vertical=true, bool mipmap=true);

		Texture2D(int width, int height, GLenum internal_format, GLenum format, void* data, GLenum data_type, const TexParams& params);
		/// <summary>
		/// 返回全局的纹理储存中的目标纹理，如果尚未加载就先从文件加载到全局储存中再返回
		/// </summary>
		/// <param name="file_name"></param>
		/// <param name="model_directory"></param>
		/// <param name="_type"></param>
		/// <returns></returns>
		static Texture2D GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type, bool flip_vertical=true);
		void SetDefaultParameters();
		void GenerateMipmap();
		/// <summary>
		/// 纹理的类型
		/// </summary>
		std::string type;
		/// <summary>
		/// 所有的纹理类型
		/// </summary>
		static const std::vector<std::string> TextureTypes;

	protected:
		static std::unordered_map<std::string, Texture2D> LoadedTextures;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data);
		GLenum GLTarget() override { return GL_TEXTURE_2D; }
};




