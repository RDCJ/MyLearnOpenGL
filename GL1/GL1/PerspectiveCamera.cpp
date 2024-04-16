#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float _FOV, float _AspectRatio, float _zNear, float _zFar)
	: Camera(_zNear, _zFar), FOV(_FOV), AspectRatio(_AspectRatio)
{}

glm::mat4 PerspectiveCamera::GetProjection()
{
	// 第一个参数: fov，它表示的是视野(Field of View)，并且设置了观察空间的大小。
	// 如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
	// 第二个参数: 设置了宽高比: aspect-ratio，由视口的宽除以高所得。
	// 第三和第四个参数设置了平截头体的近和远平面
	return glm::perspective(glm::radians(FOV), AspectRatio, Z_Near, Z_Far);
}