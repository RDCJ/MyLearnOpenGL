#pragma once
#include "Texture2D.h"
#include "TextureCubeMap.h"
#include <vector>

class Material
{
	public:
		std::vector<Texture2D> textures;
		TextureCubeMap* cube_map = nullptr;

		float shininess;
		float refract_ratio;

		Material(const std::vector<Texture2D>& textures, float shininess = 0, float refract_ratio = 0)
			: textures(textures), shininess(shininess), refract_ratio(refract_ratio)
		{}

		Material(TextureCubeMap* cube_map)
		{
			this->cube_map = cube_map;
		}

		Material() = default;
	private:

	
};




