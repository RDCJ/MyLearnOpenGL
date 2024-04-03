#include "Light.h"

void Light::Update()
{
	if (this->type == Point)
		this->position = glm::vec3(std::cos(glfwGetTime() * 2) * 2, 1.5f, std::sin(glfwGetTime() * 2) * 2);
}
