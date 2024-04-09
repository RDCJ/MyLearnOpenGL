#include "Object.h"

void Object::Draw(ShaderProgram& shader)
{
	if (this->drawAble != nullptr)
		this->drawAble->Draw(shader);
}

