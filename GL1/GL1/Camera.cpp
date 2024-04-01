#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	
	glm::vec3 target = glm::vec3(0, 0, 0);
	direction = glm::normalize(position - target);

	//�ȶ���һ��������(Up Vector)�����������������͵ڶ����õ��ķ����������в�ˡ�����������˵Ľ����ͬʱ��ֱ�����������õ�ָ��x����������Ǹ�����
	glm::vec3 world_up = glm::vec3(0, 1, 0);
	right = glm::normalize(glm::cross(world_up, direction));

	up = glm::cross(direction, right);
}

glm::mat4 Camera::GetView(const glm::vec3 &target)
{
	// LookAt����: ����һ������(Look at)����Ŀ��Ĺ۲����
	// ��һ�������������λ��
	// �ڶ���������Ŀ��λ��
	// ��������������ʾ����ռ��е�������������
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
