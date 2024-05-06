#pragma once

#include "PerspectiveCamera.h"
#include <GLFW/glfw3.h>

class FPSController
{
	public:
		Camera* camera;
		GLFWwindow* window;

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

		FPSController(Camera* camera, GLFWwindow* window)
			: camera(camera), window(window)
		{}

		/// <summary>
		/// ֡����
		/// </summary>
		void Update();
		/// <summary>
		/// ��������ƶ��¼�
		/// </summary>
		void OnMouseMove(float x_offset, float y_offset);
		/// <summary>
		/// �����������¼�
		/// </summary>
		/// <param name="y_offset"></param>
		void OnMouseScroll(float y_offset);
};



