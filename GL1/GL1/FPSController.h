#pragma once

#include "PerspectiveCamera.h"
#include <GLFW/glfw3.h>

class FPSController
{
	public:
		Camera* camera;
		GLFWwindow* window;

		/// <summary>
		/// 偏航角
		/// </summary>
		float yaw = -90;
		/// <summary>
		/// 俯仰角
		/// </summary>
		float pitch = 0;
		/// <summary>
		/// 视角移动的灵敏度
		/// </summary>
		float sensitivity = 0.05f;

		FPSController(Camera* camera, GLFWwindow* window)
			: camera(camera), window(window)
		{}

		/// <summary>
		/// 帧更新
		/// </summary>
		void Update();
		/// <summary>
		/// 处理鼠标移动事件
		/// </summary>
		void OnMouseMove(float x_offset, float y_offset);
		/// <summary>
		/// 处理鼠标滚轮事件
		/// </summary>
		/// <param name="y_offset"></param>
		void OnMouseScroll(float y_offset);
};



