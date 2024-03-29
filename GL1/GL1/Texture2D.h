#pragma once
#ifndef  TEXTURE2D_H

#include <glad/glad.h>
#include "stb_image.h"
class Texture2D
{
	public:
		unsigned int GetID() { return ID; }
		Texture2D(const char* image_path);
		void SetParameters();
		void GenerateMipmap();
		/// <summary>
		/// °ó¶¨µ½GL_TEXTURE_2D
		/// </summary>
		void Bind();

	private:
		unsigned int ID;
};

#endif // ! TEXTURE2D_H



