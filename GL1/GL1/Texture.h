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
		/// ��������󶨵�ָ��������Ԫ��
		/// </summary>
		/// <param name="active_position"></param>
		void ActiveBind(GLenum tex_unit);
		/// <summary>
		/// �����������Ԫ�ϰ󶨵�����
		/// </summary>
		static void ClearAllBindTexture();
		/// <summary>
		/// ����һ����ǰδ��ռ�õ�����Ԫ
		/// </summary>
		static int GetAvaliableTexUnit();

	private:
		/// <summary>
		/// ��ǰ���������Ԫ
		/// </summary>
		static GLenum CurrentActiveTextureUnit;
		/// <summary>
		/// ��¼��ǰ����Ԫ�İ����
		/// </summary>
		static std::map<GLenum, TexBindInfo> BindMgr;
		
};



