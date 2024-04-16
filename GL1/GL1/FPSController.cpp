#include "FPSController.h"


void FPSController::Update()
{
	//float radius = 10.0f;

	//position.x = std::sin(glfwGetTime()) * radius;
	//position.z = std::cos(glfwGetTime()) * radius;

	float move_speed = 3.0f;

	if (glfwGetKey(window, GLFW_KEY_W))
		camera->position += move_speed * Time::deltaTime * camera->Front;
	else if (glfwGetKey(window, GLFW_KEY_S))
		camera->position -= move_speed * Time::deltaTime * camera->Front;
	if (glfwGetKey(window, GLFW_KEY_D))
		camera->position -= move_speed * Time::deltaTime * glm::normalize(glm::cross(camera->Up, camera->Front));
	else if (glfwGetKey(window, GLFW_KEY_A))
		camera->position += move_speed * Time::deltaTime * glm::normalize(glm::cross(camera->Up, camera->Front));

	if (glfwGetKey(window, GLFW_KEY_SPACE))
		camera->position += move_speed * Time::deltaTime * glm::vec3(0, 1, 0);
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		camera->position -= move_speed * Time::deltaTime * glm::vec3(0, 1, 0);
}

void FPSController::OnMouseMove(float x_offset, float y_offset)
{
	yaw += x_offset * sensitivity;
	pitch += y_offset * sensitivity;

	// ���Ƹ����Ƕ�
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	float pitch_radians = glm::radians(pitch), yaw_radians = glm::radians(yaw);

	glm::vec3 front;
	// ����x,z: ����������ͶӰ��xzƽ�棬�ٷֱ�ͶӰ��x��z��
	// ����y����������ͶӰ��y��
	front.x = std::cos(pitch_radians) * std::cos(yaw_radians);
	front.y = std::sin(pitch_radians);
	front.z = std::cos(pitch_radians) * std::sin(yaw_radians);

	camera->UpdateFront(front);
}

void FPSController::OnMouseScroll(float y_offset)
{
	camera->FOV = glm::clamp(camera->FOV - y_offset, 1.0f, 100.0f);
}