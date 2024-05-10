#pragma once

#include <glad/glad.h>
#include <map>
#include "GLObject.h"
#include <iostream>

typedef std::map<GLenum, GLenum> TexParams;

struct TexBindInfo
{
	GLenum tex_type;
	unsigned int tex_id;
};

class Texture : public GLObject
{
	public:
		static const GLenum TexUnitMin = GL_TEXTURE0;
		static const GLenum TexUnitMax= GL_TEXTURE31;
		void SetParameters(GLenum option, GLenum value);
		void SetParameters(const TexParams& params);
		void Bind(GLenum target, unsigned int _ID) override;
		void Generate() override;
		/// <summary>
		/// 将该纹理绑定到指定的纹理单元上
		/// </summary>
		/// <param name="active_position"></param>
		void ActiveBind(GLenum tex_unit);
		/// <summary>
		/// 清除所有纹理单元上绑定的纹理
		/// </summary>
		static void ClearAllBindTexture();
		/// <summary>
		/// 返回一个当前未被占用的纹理单元
		/// </summary>
		static int GetAvaliableTexUnit();

	private:
		/// <summary>
		/// 当前激活的纹理单元
		/// </summary>
		static GLenum CurrentActiveTextureUnit;
		/// <summary>
		/// 记录当前纹理单元的绑定情况
		/// </summary>
		static std::map<GLenum, TexBindInfo> BindMgr;
		
};



