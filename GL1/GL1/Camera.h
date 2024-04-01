#pragma once
#ifndef CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Time.h"

class Camera
{
	public:
		GLFWwindow* window;
		/// <summary>
		/// �����λ��: ����ռ���һ��ָ�������λ�õ�����
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// �������z��������(�������������ϵ)
		/// </summary>
		glm::vec3 Front;
		/// <summary>
		/// ������ռ��x���������(�������������ϵ)
		/// </summary>
		glm::vec3 Right;
		/// <summary>
		/// ������ռ��y���������(�������������ϵ)
		/// </summary>
		glm::vec3 Up;
		/// <summary>
		/// ƫ����
		/// </summary>
		float yaw = -90;
		/// <summary>
		/// ������
		/// </summary>
		float pitch = 0;
		/// <summary>
		/// �ӽ��ƶ���������
		/// </summary>
		float sensitivity = 0.05f;

		Camera(GLFWwindow* _window);

		/// <summary>
		/// ���ؿ��ŵ�ǰ����Ĺ۲����
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView();
		/// <summary>
		/// �����������Ŀ��λ�ã������ع۲����
		/// </summary>
		/// <param name="target">Ŀ��λ��</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// ֡����
		/// </summary>
		void Update();
		/// <summary>
		/// ��������ƶ��¼�
		/// </summary>
		void OnMouseMove(float x_offset, float y_offset);

	private:
		
};
#endif // !CAMERA_H



