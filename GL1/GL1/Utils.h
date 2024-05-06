#pragma once

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

	template <typename T>
	static size_t MemorySize(const std::vector<T>& v)
	{
		return v.size() * sizeof(T);
	}

	/// <summary>
	/// 计算pos1的切线空间，pos2和pos3的顺序不影响结果
	/// </summary>
	/// <param name="pos1"></param>
	/// <param name="pos2"></param>
	/// <param name="pos3"></param>
	/// <param name="uv1"></param>
	/// <param name="uv2"></param>
	/// <param name="uv3"></param>
	/// <returns></returns>
	static glm::vec3 CalcTBN(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
		glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
};



