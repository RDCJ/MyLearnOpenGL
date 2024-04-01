#include "Camera.h"

Camera::Camera(GLFWwindow* _window)
{
	this->window = _window;

	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);
	Front = glm::normalize(target - position);

	//�ȶ���һ��������(Up Vector)�����������������͵ڶ����õ��ķ����������в�ˡ�����������˵Ľ����ͬʱ��ֱ�����������õ�ָ��x����������Ǹ�����
	glm::vec3 world_up = glm::vec3(0, 1, 0);
	Right = glm::normalize(glm::cross(world_up, Front));

	Up = glm::cross(Front, Right);

	//std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
}

glm::mat4 Camera::GetView()
{
	// LookAt����: ����һ������(Look at)����Ŀ��Ĺ۲����
	// ��һ�������������λ��
	// �ڶ���������Ŀ��λ��
	// ������������������
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

	// ���Ƹ����Ƕ�
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	float pitch_radians = glm::radians(pitch), yaw_radians = glm::radians(yaw);

	glm::vec3 front;
	// ����x,z: ����������ͶӰ��xzƽ�棬�ٷֱ�ͶӰ��x��z��
	// ����y����������ͶӰ��y��
	front.x = std::cos(pitch_radians) * std::cos(yaw_radians);
	front.y = std::sin(pitch_radians);
	front.z = std::cos(pitch_radians) * std::sin(yaw_radians);

	Front = glm::normalize(front);
	//std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
}
