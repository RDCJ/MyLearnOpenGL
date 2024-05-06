#include "PointShadow.h"

const int PointShadow::FOV = 90;
const std::vector<glm::vec3> PointShadow::six_dir = {
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(-1.0, 0.0, 0.0),
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(0.0 ,-1.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0),
		glm::vec3(0.0, 0.0, -1.0)
};
const std::vector<glm::vec3> PointShadow::six_up = {
		glm::vec3(0.0, -1.0, 0.0),
		glm::vec3(0.0, -1.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0),
		glm::vec3(0.0, 0.0, -1.0),
		glm::vec3(0.0, -1.0, 0.0),
		glm::vec3(0.0, -1.0, 0.0)
};

PointShadow::PointShadow(int width, int height, Light* light, TexParams& depth_tex_params, float z_near, float z_far): Shadow(width, height, light)
{
	InitTexture(depth_tex_params);
	camera = new PerspectiveCamera(FOV, width / (float)height, z_near, z_far);
}

void PointShadow::InitTexture(TexParams& depth_tex_params)
{
	depth_map_buffer.AddTextureCubMap(GL_DEPTH_COMPONENT, GL_FLOAT, 0, depth_tex_params);
	depth_map_buffer.CheckStatus();
}
