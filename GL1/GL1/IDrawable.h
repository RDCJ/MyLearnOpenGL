#pragma once
#ifndef IDRAWABLE_H
#include "ShaderProgram.h"
class IDrawable
{
	public:
		void virtual Draw(ShaderProgram& shader) = 0;
};
#endif // !DRAWABLE_H


