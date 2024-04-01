#include "Camera.h"

Camera::Camera(GLFWwindow* _window)
{
	this->window = _window;

	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);
	Front = glm::normalize(target - position);

	//先定义一个上向量(Up Vector)。接下来把上向量和第二步得到的方向向量进行叉乘。两个向量叉乘的结果会同时垂直于两向量，得到指向x轴正方向的那个向量
	glm::vec3 world_up = glm::vec3(0, 1, 0);
	Right = glm::normalize(glm::cross(world_up, Front));

	Up = glm::cross(Front, Right);

	//std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
}

glm::mat4 Camera::GetView()
{
	// LookAt矩阵: 创建一个看着(Look at)给定目标的观察矩阵
	// 第一个参数：摄像机位置
	// 第二个参数：目标位置
	// 第三个参数：上向量
	return glm::lookAt(
		position, position + Front, this->Up
	);
}

glm::mat4 Camera::GetView(const glm::vec3 &target)
{
	Front = glm::normalize(target - position);
	return this->GetView();
}

void Camera::Update()
{
	//float radius = 10.0f;

	//position.x = std::sin(glfwGetTime()) * radius;
	//position.z = std::cos(glfwGetTime()) * radius;

	float move_speed = 1.0f;

	if (glfwGetKey(window, GLFW_KEY_W))
		position -= move_speed * Time::deltaTime * Front;
	else if (glfwGetKey(window, GLFW_KEY_S))
		position += move_speed * Time::deltaTime * Front;
	if (glfwGetKey(window, GLFW_KEY_D))
		position += move_speed * Time::deltaTime * glm::normalize(glm::cross(Up, Front));
	else if (glfwGetKey(window, GLFW_KEY_A))
		position -= move_speed * Time::deltaTime * glm::normalize(glm::cross(Up, Front));
}

void Camera::OnMouseMove(float x_offset, float y_offset)
{
	yaw += x_offset * sensitivity;
	pitch += y_offset * sensitivity;

	// 限制俯仰角度
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	float pitch_radians = glm::radians(pitch), yaw_radians = glm::radians(yaw);

	glm::vec3 front;
	// 计算x,z: 方向向量先投影到xz平面，再分别投影到x，z轴
	// 计算y：方向向量投影到y轴
	front.x = std::cos(pitch_radians) * std::cos(yaw_radians);
	front.y = std::sin(pitch_radians);
	front.z = std::cos(pitch_radians) * std::sin(yaw_radians);

	Front = glm::normalize(front);
	//std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
}
