#pragma once
#ifndef LIGHT_H
#include <glm/glm.hpp>

enum LightType
{
	Directional = 0,
	Point = 1,
	Spot = 2
};

class Light
{
	public:
		LightType type = Directional;
		glm::vec3 position = glm::vec3(0);
		glm::vec3 direction = glm::vec3(0);
		
		glm::vec3 ambient = glm::vec3(0);
		glm::vec3 diffuse = glm::vec3(0);
		glm::vec3 specular = glm::vec3(0);

		Light(LightType type = Directional, glm::vec3 position = glm::vec3(0), glm::vec3 direction = glm::vec3(0), 
				glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), glm::vec3 specular = glm::vec3(0))
			: type(type), position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular)
		{}
	private:

};
#endif // !LIGHT_H




