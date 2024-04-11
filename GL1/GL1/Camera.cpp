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
	// LookAt����: ����һ������(Look at)����Ŀ��Ĺ۲����
	// ��һ�������������λ��
	// �ڶ���������Ŀ��λ��
	// ������������������
	return glm::lookAt(
		position, position + Front, this->Up
	);

	//glm::vec4 R = glm::vec4(Right, 0);
	//glm::vec4 U = glm::vec4(Up, 0);
	//glm::vec4 F = glm::vec4(-Front, 0); // ע������Ӹ���
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
	//��һ������: fov������ʾ������Ұ(Field of View)�����������˹۲�ռ�Ĵ�С��
	// �����Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
	// �ڶ�������: �����˿�߱�: aspect-ratio�����ӿڵĿ���Ը����á�
	// �����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ��
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

	// ���Ƹ����Ƕ�
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	float pitch_radians = glm::radians(pitch), yaw_radians = glm::radians(yaw);

	glm::vec3 front;
	// ����x,z: ����������ͶӰ��xzƽ�棬�ٷֱ�ͶӰ��x��z��
	// ����y����������ͶӰ��y��
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
	// glBufferSubData��Uniform�������������
	// ������Ŀ�꣬ƫ��������С������
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
