#pragma once
#ifndef CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include "UniformBuffer.h"
#include "Time.h"
#include "Utils.h"

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
		/// 视野,定义了可以看到场景中多大的范围
		/// </summary>
		float FOV; 
		/// <summary>
		/// 宽高比
		/// </summary>
		float AspectRatio;
		/// <summary>
		/// 近平面
		/// </summary>
		float Z_Near;
		/// <summary>
		/// 远平面
		/// </summary>
		float Z_Far;
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

		Camera(GLFWwindow* _window, float _FOV = 45.0f, float _AspectRatio = 1.0f, float _zNear = 0.1f, float _zFar = 100.0f);

		/// <summary>
		/// 返回看着当前方向的观察矩阵 (根据当前摄像机的位置，朝向计算)
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
		/// 返回投影矩阵 (使用FOV, AspectRatio, Z_Near, Z_Far计算)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection();
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
		/// <summary>
		/// 用direction更新Front, 并更新Right和Up
		/// </summary>
		/// <param name="direction"></param>
		void UpdateFront(glm::vec3& direction);
		/// <summary>
		/// 将view和projection填充到Uniform缓冲对象中
		/// </summary>
		void FillUniformMatrices();

		UniformBuffer uniform_matrices;

	private:
		
};
#endif // !CAMERA_H



