#pragma once
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include "stb_image.h"
#include "Texture.h"

class TextureCubeMap: public Texture
{
	public:
		TextureCubeMap() { }
		TextureCubeMap(std::vector<std::string>& img_paths);
		TextureCubeMap(int width, int height, GLenum format, void* data, GLenum data_type);
		void SetDefaultParameters();

	private:
		void ToGL(int width, int height, int nChannel, unsigned char* img_data, int position);
		GLenum GLTarget() override { return GL_TEXTURE_CUBE_MAP; }

};





