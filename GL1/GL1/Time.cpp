#include "Time.h"

float Time::deltaTime;
float Time::lastFrame;

void Time::Init()
{
	deltaTime = 0;
	lastFrame = 0;
}

void Time::Update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}


