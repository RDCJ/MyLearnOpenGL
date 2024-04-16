#pragma once
#ifndef TEXTURECUBEMAP_H
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include "stb_image.h"
#include "Texture.h"

class TextureCubeMap: public Texture
{
	public:
		TextureCubeMap(std::vector<std::string>& img_paths);
		void SetDefaultParameters();

	private:
		void ToGL(int width, int height, int nChannel, unsigned char* img_data, int position);
		GLenum GLTarget() override { return GL_TEXTURE_CUBE_MAP; }

};

#endif // !TEXTURECUBEMAP_H




