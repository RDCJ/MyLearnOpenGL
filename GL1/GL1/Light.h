#pragma once
#ifndef LIGHT_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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

		float innerCutOff = 0;
		float outerCutOff = 0;

		glm::vec3 ambient = glm::vec3(0);
		glm::vec3 diffuse = glm::vec3(0);
		glm::vec3 specular = glm::vec3(0);

		float constant = 1.0;
		float linear = 0;
		float quadratic = 0;

		void Update();

		static Light* CreateDirectional(glm::vec3 _position, glm::vec3 _direction);
		static Light* CreatePoint(glm::vec3 _position, float _constant = 1, float _linear = 0, float _quadratic=0);
		static Light* CreateSpot(glm::vec3 _position, glm::vec3 _direction, float _innerCutOff, float _outerCutOff);

	private:

};
#endif // !LIGHT_H




