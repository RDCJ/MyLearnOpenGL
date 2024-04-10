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
		/// ����ȫ�ֵ��������е�Ŀ�����������δ���ؾ��ȴ��ļ����ص�ȫ�ִ������ٷ���
		/// </summary>
		/// <param name="file_name"></param>
		/// <param name="model_directory"></param>
		/// <param name="_type"></param>
		/// <returns></returns>
		static Texture2D GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type, bool flip_vertical=true);
		void SetParameters();
		void GenerateMipmap();
		/// <summary>
		/// �󶨵�GL_TEXTURE_2D
		/// </summary>
		void Bind();
		/// <summary>
		/// ���������
		/// </summary>
		std::string type;
		/// <summary>
		/// ���е���������
		/// </summary>
		static const std::vector<std::string> TextureTypes;


	private:
		static std::unordered_map<std::string, Texture2D> LoadedTextures;
		unsigned int ID = 0;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data);
		
};

#endif // ! TEXTURE2D_H



