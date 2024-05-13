#include "Texture2D.h"

const std::vector<std::string> Texture2D::TextureTypes
{
	"texture_diffuse",
	"texture_specular",
	"texture_emission",
	"texture_ambient",
	"texture_normal",
	"texture_parallax"
};

const TexParams Texture2D::DefaultParams = 
{
	{GL_TEXTURE_WRAP_S, GL_REPEAT},
	{GL_TEXTURE_WRAP_T, GL_REPEAT},
	{GL_TEXTURE_MIN_FILTER, GL_LINEAR},
	{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
};

std::unordered_map<std::string, Texture2D> Texture2D::LoadedTextures;

Texture2D::Texture2D(const char* image_path, std::string _type, bool flip_vertical, bool mipmap): type(_type)
{
	stbi_set_flip_vertically_on_load(flip_vertical);
	int width, height, nrChannels;
	//��ȡͼ���ļ����������ļ�·������ȡ��߶Ⱥ���ɫͨ���ĸ���
	unsigned char* img_data = stbi_load(image_path, &width, &height, &nrChannels, 0);

	if (img_data)
	{
		Generate();
		BindSelf();
		ToGL(width, height, nrChannels, img_data);
		if (mipmap)
			GenerateMipmap();
		SetDefaultParameters();
		UnbindSelf();
	}
	else
		std::cout<< "Texture failed to load at path: " << image_path << std::endl;

	stbi_image_free(img_data);
}

Texture2D::Texture2D(int width, int height, GLenum internal_format, GLenum format, void* data, GLenum data_type, const TexParams& params)
{
	Generate();
	BindSelf();
	glTexImage2D(GLTarget(), 0, internal_format, width, height, 0, format, data_type, data);
	SetParameters(params);
	UnbindSelf();
}

Texture2D Texture2D::GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type, bool flip_vertical)
{
	std::string file_path = model_directory + "/" + std::string(file_name);

	auto it = Texture2D::LoadedTextures.find(file_path);
	if (it != Texture2D::LoadedTextures.end())
	{
		return (*it).second;
	}
	else
	{
		Texture2D texture(file_path.c_str(), _type, flip_vertical);
		Texture2D::LoadedTextures[file_path] = texture;
		return Texture2D::LoadedTextures[file_path];
	}
}

void Texture2D::SetDefaultParameters()
{
	// glTexParameteri�԰󶨵���������������
	// �ڶ���������Ҫ����ָ�����õ�ѡ����Ӧ�õ������ᣬWRAP��ʾ���û��Ʒ�ʽ��S,T��Ӧx,y��
	glTexParameteri(GLTarget(), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GLTarget(), GL_TEXTURE_WRAP_T, GL_REPEAT);
	// MIN, MAG��ʾ�����зŴ�(Magnify)����С(Minify)������ʱ���������������˵�ѡ�GL_LINEAR�����Թ���
	glTexParameteri(GLTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GLTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Texture2D::GenerateMipmap()
{
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::ToGL(int width, int height, int nChannel, unsigned char* img_data)
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
	/*glTexImage2D����������ǰ�󶨵��������ͻᱻ����������ͼ��
	��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
	�ڶ�������Ϊ����ָ��mipmap�ļ���0��ʾ��������
	��������������OpenGL��������Ϊ���ָ�ʽ
	���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�
	����������Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
	���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�
	���һ��������������ͼ������
		*/
	glTexImage2D(GLTarget(), 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
}
