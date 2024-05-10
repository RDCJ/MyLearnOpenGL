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
	// ��Ϊ��������ͼ������6������ÿ����һ����������Ҫ����glTexImage2D����6��
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
	// GL_TEXTURE_WRAP_RΪ�����R���������˻��Ʒ�ʽ������Ӧ��������ĵ�����ά�ȣ���λ�õ�zһ������
	// ���ǽ����Ʒ�ʽ����ΪGL_CLAMP_TO_EDGE��������Ϊ���ô���������֮�������������ܲ��ܻ���һ���棨����һЩӲ�����ƣ���
	// ����ͨ��ʹ��GL_CLAMP_TO_EDGE��OpenGL�������Ƕ�������֮�������ʱ����Զ�������ǵı߽�ֵ
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


void TextureCubeMap::ToGL(int width, int height, int nChannel, unsigned char* img_data, int position)
{
	// ����ͨ����ʹ����Ӧ�ĸ�ʽ
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
	
	// OpenGL�������ṩ��6�����������Ŀ��, ��Ӧ��������ͼ��һ����
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X		��
	//GL_TEXTURE_CUBE_MAP_NEGATIVE_X	��
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Y		��
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	��
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Z		��
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	ǰ
	// ���Ǳ����intֵ�����Ե����ģ����Դ�GL_TEXTURE_CUBE_MAP_POSITIVE_X��ʼ��������
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + position, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
}
