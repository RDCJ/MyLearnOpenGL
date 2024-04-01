#pragma once
#ifndef CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>

class Camera
{
	public:
		/// <summary>
		/// 摄像机位置: 世界空间中一个指向摄像机位置的向量
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// 摄像机指向的方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 direction;
		/// <summary>
		/// 摄像机空间的x轴的正方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 right;
		/// <summary>
		/// 摄像机空间的y轴的正方向(相对于世界坐标系)
		/// </summary>
		glm::vec3 up;
		Camera();

		/// <summary>
		/// 返回看着给定目标的观察矩阵
		/// </summary>
		/// <param name="target">目标位置</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// 帧更新
		/// </summary>
		void Update();

	private:

};
#endif // !CAMERA_H



