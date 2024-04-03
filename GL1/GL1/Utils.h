#pragma once
#ifndef UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>
#include <iostream>

class Utils
{
public:
	static void PrintVec3(glm::vec3& vec);
	static void PrintMat4(glm::mat4& mat);

	static const glm::vec3 Up;
	static const glm::vec3 Down;
	static const glm::vec3 Left;
	static const glm::vec3 Right;
	static const glm::vec3 Forward;
	static const glm::vec3 Back;
};


#endif // !UTILS_H



