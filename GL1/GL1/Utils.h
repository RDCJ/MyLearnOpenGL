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
};

#endif // !UTILS_H



