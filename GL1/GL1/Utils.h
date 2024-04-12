#pragma once
#ifndef UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdlib>
#include <GLFW/glfw3.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Utils
{
public:
	static void PrintVec3(glm::vec3& vec);
	static void PrintMat4(glm::mat4& mat);
	static glm::vec3 ToGlmV3(aiVector3D &v);
	static glm::vec2 ToGlmV2(aiVector3D& v);

	static const glm::vec3 Up;
	static const glm::vec3 Down;
	static const glm::vec3 Left;
	static const glm::vec3 Right;
	static const glm::vec3 Forward;
	static const glm::vec3 Back;
	
	static int RandomInt(int a, int b);
	static float RandomFloat(float a, float b);
};


#endif // !UTILS_H



