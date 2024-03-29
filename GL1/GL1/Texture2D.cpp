#include "Texture2D.h"

Texture2D::Texture2D(const char* image_path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	//��ȡͼ���ļ����������ļ�·������ȡ��߶Ⱥ���ɫͨ���ĸ���
	unsigned char* img_data = stbi_load(image_path, &width, &height, &nrChannels, 0);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	/*glTexImage2D����������ǰ�󶨵��������ͻᱻ����������ͼ��
	��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
	�ڶ�������Ϊ����ָ��mipmap�ļ���0��ʾ��������
	��������������OpenGL��������Ϊ���ָ�ʽ
	���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�
	����������Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
	���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�
	���һ��������������ͼ������
		*/
	if (nrChannels == 4) // ����ͨ����ʹ����Ӧ�ĸ�ʽ
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);

	stbi_image_free(img_data);
}

void Texture2D::SetParameters()
{
	Bind();
	// glTexParameteri�԰󶨵���������������
	// �ڶ���������Ҫ����ָ�����õ�ѡ����Ӧ�õ������ᣬWRAP��ʾ���û��Ʒ�ʽ��S,T��Ӧx,y��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// MIN, MAG��ʾ�����зŴ�(Magnify)����С(Minify)������ʱ���������������˵�ѡ�GL_LINEAR�����Թ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::GenerateMipmap()
{
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::Bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}
