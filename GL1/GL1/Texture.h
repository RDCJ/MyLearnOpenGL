#pragma once

#include <glad/glad.h>
#include <map>
#include "GLObject.h"

typedef std::map<GLenum, GLenum> TexParams;

class Texture : public GLObject
{
	public:
		void SetParameters(GLenum option, GLenum value);
		void SetParameters(const TexParams& params);
		void Bind(GLenum target, unsigned int _ID) override;
		void Generate() override;
};



