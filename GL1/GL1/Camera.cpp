#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(float _zNear, float _zFar):
	Z_Near(_zNear), Z_Far(_zFar)
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

inline void Camera::UpdateFront(const glm::vec3& direction)
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
