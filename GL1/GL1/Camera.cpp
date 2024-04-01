#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);
	direction = glm::normalize(position - target);

	//先定义一个上向量(Up Vector)。接下来把上向量和第二步得到的方向向量进行叉乘。两个向量叉乘的结果会同时垂直于两向量，得到指向x轴正方向的那个向量
	glm::vec3 world_up = glm::vec3(0, 1, 0);
	right = glm::normalize(glm::cross(world_up, direction));

	up = glm::cross(direction, right);
}

glm::mat4 Camera::GetView(const glm::vec3 &target)
{
	// LookAt矩阵: 创建一个看着(Look at)给定目标的观察矩阵
	// 第一个参数：摄像机位置
	// 第二个参数：目标位置
	// 第三个参数：表示世界空间中的上向量的向量
	return glm::lookAt(
		position, target, glm::vec3(0, 1, 0)
	);

}

void Camera::Update()
{
	float radius = 10.0f;

	position.x = std::sin(glfwGetTime()) * radius;
	position.z = std::cos(glfwGetTime()) * radius;
}
