#pragma once
#include <glad/glad.h>

class GLObject
{
	public:
		unsigned int GetID()
		{
			return ID;
		}

		virtual void Bind(GLenum target, unsigned int _ID) = 0;

		/// <summary>
		/// °ó¶¨
		/// </summary>
		void BindSelf()
		{
			Bind(GLTarget(), ID);
		}

		void UnbindSelf()
		{
			Bind(GLTarget(), 0);
		}

		template<typename T>
		static void Unbind()
		{
			GLObject* p = new T();
			p->Bind(p->GLTarget(), 0);
		}

		template<typename T>
		static void Unbind(T& p)
		{
			p.Bind(p.GLTarget(), 0);
		}

	protected:
		unsigned int ID = 0;
		virtual void Generate() = 0;
		virtual GLenum GLTarget() = 0;
};

