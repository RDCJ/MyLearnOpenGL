#pragma once
#include "IDrawable.h"
#include "Transform.h"
#include "Material.h"

class Object: IDrawable
{
	public:
		IDrawable* drawAble;
		Transform transform;
		Material material;


		// Í¨¹ý Drawable ¼Ì³Ð
		void Draw(ShaderProgram& shader) override;

		Object(IDrawable* drawAble, const Transform& transform)
			: drawAble(drawAble), transform(transform)
		{};

		Object(IDrawable* drawAble, const Transform& transform, const Material& material)
			: drawAble(drawAble), transform(transform), material(material)
		{};

		Object(IDrawable* drawAble)
			: drawAble(drawAble)
		{
			transform = Transform();
		};

		Object() = default;
};



