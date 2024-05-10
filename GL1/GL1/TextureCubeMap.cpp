#include "TextureCubeMap.h"

TextureCubeMap::TextureCubeMap(std::vector<std::string>& img_paths)
{
	Generate();
	BindSelf();

	if (img_paths.size() < 6)
	{
		std::cout << "Length of img_paths is not enough for cube map: " << img_paths.size() << std::endl;
	}
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	// 因为立方体贴图包含有6个纹理，每个面一个，我们需要调用glTexImage2D函数6次
	for (int i = 0; i < img_paths.size(); i++)
	{
		unsigned char* img_data = stbi_load(img_paths[i].c_str(), &width, &height, &nrChannels, 0);
		if (img_data)
		{
			ToGL(width, height, nrChannels, img_data, i);
		}
		else
			std::cout << "Texture failed to load at path: " << img_paths[i] << std::endl;

		stbi_image_free(img_data);
	}
	SetDefaultParameters();
	UnbindSelf();
}

TextureCubeMap::TextureCubeMap(int width, int height, GLenum format, void* data, GLenum data_type)
{
	Generate();
	BindSelf();
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, data_type, data);
	}
	UnbindSelf();
}

void TextureCubeMap::SetDefaultParameters()
{
	GLenum target = GLTarget();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// GL_TEXTURE_WRAP_R为纹理的R坐标设置了环绕方式，它对应的是纹理的第三个维度（和位置的z一样）。
	// 我们将环绕方式设置为GL_CLAMP_TO_EDGE，这是因为正好处于两个面之间的纹理坐标可能不能击中一个面（由于一些硬件限制），
	// 所以通过使用GL_CLAMP_TO_EDGE，OpenGL将在我们对两个面之间采样的时候，永远返回它们的边界值
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


void TextureCubeMap::ToGL(int width, int height, int nChannel, unsigned char* img_data, int position)
{
	// 根据通道数使用相应的格式
	GLenum format = GL_RGB;
	switch (nChannel)
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}
	
	// OpenGL给我们提供了6个特殊的纹理目标, 对应立方体贴图的一个面
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X		右
	//GL_TEXTURE_CUBE_MAP_NEGATIVE_X	左
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Y		上
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	下
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Z		后
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	前
	// 它们背后的int值是线性递增的，可以从GL_TEXTURE_CUBE_MAP_POSITIVE_X开始遍历它们
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + position, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
}
