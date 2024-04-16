#pragma once
#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H
#include "Camera.h"

class PerspectiveCamera: public Camera
{
	public:
		/// <summary>
		/// ��Ұ,�����˿��Կ��������ж��ķ�Χ
		/// </summary>
		float FOV;
		/// <summary>
		/// ��߱�
		/// </summary>
		float AspectRatio;

		PerspectiveCamera(float _FOV = 45.0f, float _AspectRatio = 1.0f, float _zNear = 0.1f, float _zFar = 100.0f);
		/// <summary>
		/// ����ͶӰ���� (ʹ��FOV, AspectRatio, Z_Near, Z_Far����)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection() override;

};

#endif // !PERSPECTIVECAMERA_H




