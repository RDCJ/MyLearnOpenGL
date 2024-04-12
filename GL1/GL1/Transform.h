#pragma once
#ifndef TRANSFORM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
	public:
		glm::vec3 position;
		glm::vec3 scale;

		glm::vec3 rotate_axis;
		float rotate_angle;

		glm::mat4 GetModel() const
		{
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, position);
			model = glm::scale(model, scale);
			if (glm::length(rotate_axis) > 0)
				model = glm::rotate(model, glm::radians(rotate_angle), rotate_axis);
			return model;
		}

		Transform()
		{
			position = glm::vec3(0);
			scale = glm::vec3(1);
			rotate_axis = glm::vec3(0);
			rotate_angle = 0;
		}

		Transform(const glm::vec3& position, const glm::vec3& scale)
			: position(position), scale(scale), rotate_axis(glm::vec3(0)), rotate_angle(0)
		{}

		Transform(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotate_axis, float rotate_angle)
			: position(position), scale(scale), rotate_axis(rotate_axis), rotate_angle(rotate_angle)
		{}
};
#endif // !TRANSFORM



