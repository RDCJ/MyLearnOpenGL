#pragma once
#ifndef CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>

#include "Time.h"

class Camera
{
	public:
		GLFWwindow* window;
		/// <summary>
		/// �����λ��: ����ռ���һ��ָ�������λ�õ�����
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// �������z��������(�������������ϵ)
		/// ע�⣺�����������ϵ�£������ָ��z�Ḻ����direction����Ϊָ��z��������
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
		Camera(GLFWwindow* _window);

		/// <summary>
		/// ���ؿ��ŵ�ǰ����Ĺ۲����
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView();
		/// <summary>
		/// �����������Ŀ��λ�ã������ع۲����
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



