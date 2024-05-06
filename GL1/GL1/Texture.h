#pragma once

#include <glad/glad.h>
#include <map>

typedef std::map<GLenum, GLenum> TexParams;

class Texture
{
	public:
		unsigned int GetID() { return ID; }
		void SetParameters(GLenum option, GLenum value);
		void SetParameters(const TexParams& params);
		/// <summary>
		/// °ó¶¨
		/// </summary>
		void Bind();
		/// <summary>
		/// ½â°ó
		/// </summary>
		void Unbind();

	private:
	protected:
		unsigned int ID = 0;
		virtual GLenum GLTarget() = 0;
};



