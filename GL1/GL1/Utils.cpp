#include "Utils.h"

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
