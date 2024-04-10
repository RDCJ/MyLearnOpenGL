#include "Texture2D.h"

const std::vector<std::string> Texture2D::TextureTypes
{
	"texture_diffuse",
	"texture_specular",
	"texture_emission",
	"texture_ambient"
};

std::unordered_map<std::string, Texture2D> Texture2D::LoadedTextures;

Texture2D::Texture2D(const char* image_path, std::string _type): type(_type)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	//读取图像文件，参数：文件路径，宽度、高度和颜色通道的个数
	unsigned char* img_data = stbi_load(image_path, &width, &height, &nrChannels, 0);

	if (img_data)
	{
		ToGL(width, height, nrChannels, img_data);
		GenerateMipmap();
		SetParameters();
	}
	else
		std::cout<< "Texture failed to load at path: " << image_path << std::endl;

	stbi_image_free(img_data);
}

Texture2D Texture2D::GetTexture2D(const char* file_name, const std::string& model_directory, std::string _type)
{
	std::string file_path = model_directory + "/" + std::string(file_name);

	auto it = Texture2D::LoadedTextures.find(file_path);
	if (it != Texture2D::LoadedTextures.end())
	{
		return (*it).second;
	}
	else
	{
		Texture2D texture(file_path.c_str(), _type);
		Texture2D::LoadedTextures[file_path] = texture;
		return Texture2D::LoadedTextures[file_path];
	}
}

void Texture2D::SetParameters()
{
	// glTexParameteri对绑定的纹理对象进行配置
	// 第二个参数需要我们指定设置的选项与应用的纹理轴，WRAP表示配置环绕方式，S,T对应x,y轴
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// MIN, MAG表示当进行放大(Magnify)和缩小(Minify)操作的时候可以设置纹理过滤的选项，GL_LINEAR：线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

void Texture2D::ToGL(int width, int height, int nChannel, unsigned char* img_data)
{
	glGenTextures(1, &ID);
	Bind();

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
	/*glTexImage2D生成纹理，当前绑定的纹理对象就会被附加上纹理图像
	第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
	第二个参数为纹理指定mipmap的级别。0表示基本级别。
	第三个参数告诉OpenGL把纹理储存为何种格式
	第四个和第五个参数设置最终的纹理的宽度和高度。
	第六个参数应该总是被设为0（历史遗留的问题）。
	第七第八个参数定义了源图的格式和数据类型。
	最后一个参数是真正的图像数据
		*/
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
}
