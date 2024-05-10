#include "Texture.h"

GLenum Texture::CurrentActiveTextureUnit = GL_TEXTURE0;
std::map<GLenum, TexBindInfo> Texture::BindMgr;

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
	auto it = BindMgr.find(CurrentActiveTextureUnit);
	if (it != BindMgr.end())
	{
		it->second.tex_type = this->GLTarget();
		it->second.tex_id = _ID;
	}
	else
	{
		BindMgr[CurrentActiveTextureUnit] = TexBindInfo{this->GLTarget(), _ID};
	}
}

void Texture::Generate()
{
	glGenTextures(1, &ID);
}

void Texture::ActiveBind(GLenum tex_unit)
{
	CurrentActiveTextureUnit = tex_unit;
	glActiveTexture(CurrentActiveTextureUnit);
	this->BindSelf();
}

void Texture::ClearAllBindTexture()
{
	for (GLenum unit = TexUnitMin; unit <= TexUnitMax; ++unit)
	{
		glActiveTexture(unit);

		// 清除2D纹理绑定
		glBindTexture(GL_TEXTURE_2D, 0);

		// 清除Cube Map纹理绑定
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		BindMgr.erase(unit);
	}
	glActiveTexture(TexUnitMin);
	CurrentActiveTextureUnit = TexUnitMin;
}

int Texture::GetAvaliableTexUnit()
{
	for (GLenum unit = TexUnitMin; unit <= TexUnitMax; ++unit)
	{
		auto it = BindMgr.find(unit);
		if (it == BindMgr.end() || it->second.tex_id == 0)
			return unit;
	}
	std::cout<< "All texture units are unavaliable"<<std::endl;
	return 0;
}
