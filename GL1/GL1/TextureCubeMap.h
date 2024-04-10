#pragma once
#ifndef TEXTURECUBEMAP_H
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include "stb_image.h"

class TextureCubeMap
{
	public:
		unsigned int GetID() { return ID; }
		TextureCubeMap(std::vector<std::string>& img_paths);
		void SetParameters();
		void Bind();

	private:
		unsigned int ID;
		void ToGL(int width, int height, int nChannel, unsigned char* img_data, int position);

};

#endif // !TEXTURECUBEMAP_H




