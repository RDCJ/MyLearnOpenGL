#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(GLFWwindow* _window, float _FOV, float _AspectRatio, float _zNear, float _zFar): 
	FOV(_FOV), AspectRatio(_AspectRatio), window(_window), Z_Near(_zNear), Z_Far(_zFar)
{
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);

	glm::vec3 front = target - position;

	UpdateFront(front);

	uniform_matrices = UniformBuffer("Matrices", 0);
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

	//glm::vec4 R = glm::vec4(Right, 0);
	//glm::vec4 U = glm::vec4(Up, 0);
	//glm::vec4 F = glm::vec4(-Front, 0); // 注意这里加负号
	//glm::vec4 C = glm::vec4(0, 0, 0, 1);
	//glm::mat4 A = glm::transpose(glm::mat4(R, U, F, C));

	//glm::mat4 B = glm::mat4(1);
	//B[3][0] = -position.x;
	//B[3][1] = -position.y;
	//B[3][2] = -position.z;

	//////auto result2 = glm::transpose(A * B);
	//auto result2 = A * B;

	//return result2;
}

glm::mat4 Camera::GetView(const glm::vec3 &target)
{
	glm::vec3 front = target - position;
	UpdateFront(front);
	return this->GetView();
}

glm::mat4 Camera::GetProjection()
{
	//第一个参数: fov，它表示的是视野(Field of View)，并且设置了观察空间的大小。
	// 如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
	// 第二个参数: 设置了宽高比: aspect-ratio，由视口的宽除以高所得。
	// 第三和第四个参数设置了平截头体的近和远平面
	return glm::perspective(glm::radians(FOV), AspectRatio, Z_Near, Z_Far);
}

void Camera::Update()
{
	//float radius = 10.0f;

	//position.x = std::sin(glfwGetTime()) * radius;
	//position.z = std::cos(glfwGetTime()) * radius;

	float move_speed = 3.0f;

	if (glfwGetKey(window, GLFW_KEY_W))
		position += move_speed * Time::deltaTime * Front;
	else if (glfwGetKey(window, GLFW_KEY_S))
		position -= move_speed * Time::deltaTime * Front;
	if (glfwGetKey(window, GLFW_KEY_D))
		position -= move_speed * Time::deltaTime * glm::normalize(glm::cross(Up, Front));
	else if (glfwGetKey(window, GLFW_KEY_A))
		position += move_speed * Time::deltaTime * glm::normalize(glm::cross(Up, Front));

	if (glfwGetKey(window, GLFW_KEY_SPACE))
		position += move_speed * Time::deltaTime * glm::vec3(0, 1, 0);
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		position -= move_speed * Time::deltaTime * glm::vec3(0, 1, 0);
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

	UpdateFront(front);
}

void Camera::OnMouseScroll(float y_offset)
{
	FOV = glm::clamp(FOV - y_offset, 1.0f, 100.0f);
}

inline void Camera::UpdateFront(glm::vec3& direction)
{
	Front = glm::normalize(direction);

	Right = glm::normalize(glm::cross(Front, glm::vec3(0, 1, 0)));
	Up = glm::cross(Right, Front);
}

void Camera::FillUniformMatrices()
{
	uniform_matrices.Bind();
	auto projection = GetProjection();
	auto view = GetView();
	// glBufferSubData向Uniform缓冲中添加数据
	// 参数：目标，偏移量，大小，数据
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
