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
		/// �����λ��: ����ռ���һ��ָ�������λ�õ�����
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// �����ָ��ķ���(�������������ϵ)
		/// </summary>
		glm::vec3 direction;
		/// <summary>
		/// ������ռ��x���������(�������������ϵ)
		/// </summary>
		glm::vec3 right;
		/// <summary>
		/// ������ռ��y���������(�������������ϵ)
		/// </summary>
		glm::vec3 up;
		Camera();

		/// <summary>
		/// ���ؿ��Ÿ���Ŀ��Ĺ۲����
		/// </summary>
		/// <param name="target">Ŀ��λ��</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// ֡����
		/// </summary>
		void Update();

	private:

};
#endif // !CAMERA_H



