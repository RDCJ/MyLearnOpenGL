#pragma once
#ifndef ORTHOCAMERA_H
#define ORTHOCAMERA_H
#include "Camera.h"


class OrthoCamera: Camera
{
	public:
		float ortho_left;
		float ortho_right;
		float ortho_top;
		float ortho_bottom;

		OrthoCamera(float ortho_left, float ortho_right, float ortho_top, float ortho_bottom, float _zNear, float _zFar)
			: Camera( _zNear, _zFar), ortho_left(ortho_left), ortho_right(ortho_right), ortho_top(ortho_top), ortho_bottom(ortho_bottom)
		{}

		/// <summary>
		/// ∑µªÿÕ∂”∞æÿ’Û
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection() override;

};

#endif // !ORTHOCAMERA_H



