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

		Texture2D(int width, int height, GLenum format, void* data, GLenum data_type, const TexParams& params=TexParams());
		/// <summary>
		/// ����ȫ�ֵ��������е�Ŀ�����������δ���ؾ��ȴ��ļ����ص�ȫ�ִ������ٷ���
		/// </summary>
		/// <param name="file_name"></param>
		/// <param name="model_directory"></param>
		/// <param name="_type"></param>
		/// <returns></returns>
		static Texture2D GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type, bool flip_vertical=true);
		void SetDefaultParameters();
		void GenerateMipmap();
		/// <summary>
		/// ���������
		/// </summary>
		std::string type;
		/// <summary>
		/// ���е���������
		/// </summary>
		static const std::vector<std::string> TextureTypes;

	protected:
		static std::unordered_map<std::string, Texture2D> LoadedTextures;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data);
		GLenum GLTarget() override { return GL_TEXTURE_2D; }
};




