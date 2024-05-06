#pragma once
#include <GLFW/glfw3.h>

class Time
{
	public:
		static float deltaTime;
		static float lastFrame;
		static void Init();
		static void Update();

};




