#pragma once
#ifndef TIME_H
#include <GLFW/glfw3.h>

class Time
{
	public:
		static float deltaTime;
		static float lastFrame;
		static void Init();
		static void Update();

};
#endif // !TIME_H




