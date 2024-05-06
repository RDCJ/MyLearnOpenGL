#pragma once
#include "ShaderProgram.h"
class IDrawable
{
	public:
		void virtual Draw(ShaderProgram& shader) = 0;
		void virtual DrawInstance(ShaderProgram& shader, int num){}
};


