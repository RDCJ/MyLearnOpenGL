#pragma once
#ifndef TRANSFORM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
	public:
		glm::vec3 position;
		glm::vec3 scale;

		glm::mat4 GetModel() const
		{
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, position);
			model = glm::scale(model, scale);
			return model;
		}


		Transform()
		{
			position = glm::vec3(0);
			scale = glm::vec3(1);
		}

		Transform(const glm::vec3& position, const glm::vec3& scale)
			: position(position), scale(scale)
		{}
};
#endif // !TRANSFORM



