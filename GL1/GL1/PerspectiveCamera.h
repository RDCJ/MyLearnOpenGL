#pragma once
#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H
#include "Camera.h"

class PerspectiveCamera: public Camera
{
	public:
		/// <summary>
		/// 视野,定义了可以看到场景中多大的范围
		/// </summary>
		float FOV;
		/// <summary>
		/// 宽高比
		/// </summary>
		float AspectRatio;

		PerspectiveCamera(float _FOV = 45.0f, float _AspectRatio = 1.0f, float _zNear = 0.1f, float _zFar = 100.0f);
		/// <summary>
		/// 返回投影矩阵 (使用FOV, AspectRatio, Z_Near, Z_Far计算)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection() override;

};

#endif // !PERSPECTIVECAMERA_H




