#pragma once
#ifndef  TEXTURE2D_H

#include <glad/glad.h>
#include "stb_image.h"
#include <string>
#include <vector>
#include <iostream>

class Texture2D
{
	public:
		unsigned int GetID() { return ID; }
		Texture2D() { type = ""; ID = 0; }
		Texture2D(const char* image_path, std::string _type);
		/// <summary>
		/// 加载模型中的纹理
		/// </summary>
		/// <param name="file_name">纹理的相对路径</param>
		/// <param name="model_directory">模型文件的路径</param>
		/// <param name="gamma"></param>
		Texture2D(const char* file_name, const std::string& model_directory, std::string _type);
		void SetParameters();
		void GenerateMipmap();
		/// <summary>
		/// 绑定到GL_TEXTURE_2D
		/// </summary>
		void Bind();

		std::string type;
		static const std::vector<std::string> TextureTypes;


	private:
		unsigned int ID = 0;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data);
		
};

#endif // ! TEXTURE2D_H



