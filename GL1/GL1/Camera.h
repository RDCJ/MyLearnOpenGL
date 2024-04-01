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
		/// 摄像机位置: 世界空间中一个指向摄像机位置的向量
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// 摄像机的z轴正方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 Front;
		/// <summary>
		/// 摄像机空间的x轴的正方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 Right;
		/// <summary>
		/// 摄像机空间的y轴的正方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 Up;
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

		Camera(GLFWwindow* _window);

		/// <summary>
		/// 返回看着当前方向的观察矩阵
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView();
		/// <summary>
		/// 让摄像机看着目标位置，并返回观察矩阵
		/// </summary>
		/// <param name="target">目标位置</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// 帧更新
		/// </summary>
		void Update();
		/// <summary>
		/// 处理鼠标移动事件
		/// </summary>
		void OnMouseMove(float x_offset, float y_offset);

	private:
		
};
#endif // !CAMERA_H



