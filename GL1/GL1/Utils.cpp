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
