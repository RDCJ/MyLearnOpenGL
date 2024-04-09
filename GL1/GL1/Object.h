#pragma once
#ifndef OBJECT_H
#include "IDrawable.h"
#include "Transform.h"

class Object: IDrawable
{
	public:
		IDrawable* drawAble;
		Transform transform;

		// Í¨¹ý Drawable ¼Ì³Ð
		void Draw(ShaderProgram& shader) override;

		Object(IDrawable* drawAble, const Transform& transform)
			: drawAble(drawAble), transform(transform)
		{};

		Object(IDrawable* drawAble)
			: drawAble(drawAble)
		{
			transform = Transform();
		};

		Object() = default;
};
#endif // !OBJECT_H



