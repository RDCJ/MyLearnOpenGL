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
		/// 近平面
		/// </summary>
		float Z_Near;
		/// <summary>
		/// 远平面
		/// </summary>
		float Z_Far;

		Camera(float _zNear, float _zFar);

		/// <summary>
		/// 返回看着当前方向的观察矩阵 (根据当前摄像机的位置，朝向计算)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView() ;
		/// <summary>
		/// 让摄像机看着目标位置，并返回观察矩阵
		/// </summary>
		/// <param name="target">目标位置</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// 返回投影矩阵
		/// </summary>
		/// <returns></returns>
		virtual glm::mat4 GetProjection() { return glm::mat4(1.0f); }
		/// <summary>
		/// 用direction更新Front, 并更新Right和Up
		/// </summary>
		/// <param name="direction"></param>
		void UpdateFront(const glm::vec3& direction);
		/// <summary>
		/// 将view和projection填充到Uniform缓冲对象中
		/// </summary>
		void FillUniformMatrices();

		UniformBuffer uniform_matrices;

	private:
		
};
#endif // !CAMERA_H



