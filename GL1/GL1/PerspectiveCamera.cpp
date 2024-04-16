#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float _FOV, float _AspectRatio, float _zNear, float _zFar)
	: Camera(_zNear, _zFar), FOV(_FOV), AspectRatio(_AspectRatio)
{}

glm::mat4 PerspectiveCamera::GetProjection()
{
	// ��һ������: fov������ʾ������Ұ(Field of View)�����������˹۲�ռ�Ĵ�С��
	// �����Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
	// �ڶ�������: �����˿�߱�: aspect-ratio�����ӿڵĿ���Ը����á�
	// �����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ��
	return glm::perspective(glm::radians(FOV), AspectRatio, Z_Near, Z_Far);
}