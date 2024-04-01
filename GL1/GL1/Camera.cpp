#include "Camera.h"

Camera::Camera(GLFWwindow* _window)
{
	this->window = _window;

	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);
	direction = glm::normalize(position - target);

	//�ȶ���һ��������(Up Vector)�����������������͵ڶ����õ��ķ����������в�ˡ�����������˵Ľ����ͬʱ��ֱ�����������õ�ָ��x����������Ǹ�����
	glm::vec3 world_up = glm::vec3(0, 1, 0);
	right = glm::normalize(glm::cross(world_up, direction));

	up = glm::cross(direction, right);
}

glm::mat4 Camera::GetView()
{
	// LookAt����: ����һ������(Look at)����Ŀ��Ĺ۲����
	// ��һ�������������λ��
	// �ڶ���������Ŀ��λ��
	// ������������������
	return glm::lookAt(
		position, position - direction, this->up
	);
}

glm::mat4 Camera::GetView(const glm::vec3 &target)
{
	direction = glm::normalize(position - target);
	return this->GetView();
}

void Camera::Update()
{
	//float radius = 10.0f;

	//position.x = std::sin(glfwGetTime()) * radius;
	//position.z = std::cos(glfwGetTime()) * radius;

	float move_speed = 1.0f;

	if (glfwGetKey(window, GLFW_KEY_W))
		position -= move_speed * Time::deltaTime * direction;
	else if (glfwGetKey(window, GLFW_KEY_S))
		position += move_speed * Time::deltaTime * direction;
	else if (glfwGetKey(window, GLFW_KEY_D))
		position += move_speed * Time::deltaTime * glm::normalize(glm::cross(up, direction));
	else if (glfwGetKey(window, GLFW_KEY_A))
		position -= move_speed * Time::deltaTime * glm::normalize(glm::cross(up, direction));
}
