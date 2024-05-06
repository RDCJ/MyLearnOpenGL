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
		/// ��
		/// </summary>
		void Bind();
		/// <summary>
		/// ���
		/// </summary>
		void Unbind();

	private:
	protected:
		unsigned int ID = 0;
		virtual GLenum GLTarget() = 0;
};



