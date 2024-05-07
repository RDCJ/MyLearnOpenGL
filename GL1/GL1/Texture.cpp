#include "Texture.h"

void Texture::SetParameters(GLenum option, GLenum value)
{
	BindSelf();
	glTexParameteri(GLTarget(), option, value);
}

void Texture::SetParameters(const TexParams& params)
{
	BindSelf();
	GLenum target = GLTarget();
	for (auto& param : params)
	{
		glTexParameteri(target, param.first, param.second);
	}
}

void Texture::Bind(GLenum target, unsigned int _ID)
{
	glBindTexture(target, _ID);
}

void Texture::Generate()
{
	glGenTextures(1, &ID);
}
