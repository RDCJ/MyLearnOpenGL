#include "Utils.h"

const glm::vec3 Utils::Up = glm::vec3(0, 1, 0);
const glm::vec3 Utils::Down = glm::vec3(0, -1, 0);
const glm::vec3 Utils::Left = glm::vec3(-1, 0, 0);
const glm::vec3 Utils::Right = glm::vec3(1, 0, 0);
const glm::vec3 Utils::Forward = glm::vec3(0, 0, 1);
const glm::vec3 Utils::Back = glm::vec3(0, 0, -1);


void Utils::PrintVec3(glm::vec3& vec)
{
	std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
	std::cout << std::endl;
}

void Utils::PrintMat4(glm::mat4 & mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float v = mat[j][i];
			if (std::abs(v) < 10e-4) v = 0;
			std::cout << v << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

glm::vec3 Utils::ToGlmV3(aiVector3D &v)
{
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec2 Utils::ToGlmV2(aiVector3D& v)
{
	return glm::vec2(v.x, v.y);
}

int Utils::RandomInt(int a, int b)
{
	return a + std::rand() % (b - a);
}

float Utils::RandomFloat(float a, float b)
{
	// 生成01之间的随机浮点数
	float randomFraction = std::rand() / (float)RAND_MAX;

	// 将随机浮点数映射到[a, b]范围内
	return a + randomFraction * (b - a);
}

glm::vec3 Utils::CalcTBN(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
{
	glm::vec3 e1 = pos2 - pos1;
	glm::vec3 e2 = pos3 - pos1;
	glm::vec2 duv1 = uv2 - uv1;
	glm::vec2 duv2 = uv3 - uv1;

	float k = 1.0 / (duv1.x * duv2.y - duv2.x * duv1.y);
	glm::mat2x2 UV(duv2.y, -duv2.x, -duv1.y, duv1.x);
	glm::mat3x2 E = glm::transpose(glm::mat2x3(e1, e2));

	glm::mat2x3 TB = k * UV * E;

	return glm::normalize(TB[0]);
}

