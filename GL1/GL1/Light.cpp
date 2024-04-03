#include "Light.h"

void Light::Update()
{
	if (this->type == Point)
		this->position = glm::vec3(std::cos(glfwGetTime() * 2) * 2, 1.5f, std::sin(glfwGetTime() * 2) * 2);
}

Light* Light::CreateDirectional(glm::vec3 _position, glm::vec3 _direction)
{
	Light* p = new Light();
	p->type = Directional;
	p->position = _position;
	p->direction = _direction;
	return p;
}

Light* Light::CreatePoint(glm::vec3 _position, float _constant, float _linear, float _quadratic)
{
	Light* p = new Light();
	p->type = Point;
	p->position = _position;
	p->constant = _constant;
	p->linear = _linear;
	p->quadratic = _quadratic;
	return p;
}

Light* Light::CreateSpot(glm::vec3 _position, glm::vec3 _direction, float _innerCutOff, float _outerCutOff)
{
	Light* p = new Light();
	p->type = Spot;
	p->position = _position;
	p->direction = _direction;
	p->innerCutOff = _innerCutOff;
	p->outerCutOff = _outerCutOff;
	return p;
}

