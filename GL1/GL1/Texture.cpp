#include "Texture.h"

void Texture::SetParameters(GLenum option, GLenum value)
{
	Bind();
	glTexParameteri(GLTarget(), option, value);
}

void Texture::SetParameters(const std::map<GLenum, GLenum>&params)
{
	Bind();
	GLenum target = GLTarget();
	for (auto& param : params)
	{
		glTexParameteri(target, param.first, param.second);
	}
}

void Texture::Bind()
{
	glBindTexture(GLTarget(), ID);
}
