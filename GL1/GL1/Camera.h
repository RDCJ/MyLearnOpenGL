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
		/// �����λ��: ����ռ���һ��ָ�������λ�õ�����
		/// </summary>
		glm::vec3 position;
		/// <summary>
		/// �������z��������(�������������ϵ)
		/// </summary>
		glm::vec3 Front;
		/// <summary>
		/// ������ռ��x���������(�������������ϵ)
		/// </summary>
		glm::vec3 Right;
		/// <summary>
		/// ������ռ��y���������(�������������ϵ)
		/// </summary>
		glm::vec3 Up;
		/// <summary>
		/// ��Ұ,�����˿��Կ��������ж��ķ�Χ
		/// </summary>
		float FOV; 
		/// <summary>
		/// ��߱�
		/// </summary>
		float AspectRatio;
		/// <summary>
		/// ��ƽ��
		/// </summary>
		float Z_Near;
		/// <summary>
		/// Զƽ��
		/// </summary>
		float Z_Far;
		/// <summary>
		/// ƫ����
		/// </summary>
		float yaw = -90;
		/// <summary>
		/// ������
		/// </summary>
		float pitch = 0;
		/// <summary>
		/// �ӽ��ƶ���������
		/// </summary>
		float sensitivity = 0.05f;

		Camera(GLFWwindow* _window, float _FOV = 45.0f, float _AspectRatio = 1.0f, float _zNear = 0.1f, float _zFar = 100.0f);

		/// <summary>
		/// ���ؿ��ŵ�ǰ����Ĺ۲���� (���ݵ�ǰ�������λ�ã��������)
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
		/// ����ͶӰ���� (ʹ��FOV, AspectRatio, Z_Near, Z_Far����)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection();
		/// <summary>
		/// ֡����
		/// </summary>
		void Update();
		/// <summary>
		/// ��������ƶ��¼�
		/// </summary>
		void OnMouseMove(float x_offset, float y_offset);
		/// <summary>
		/// �����������¼�
		/// </summary>
		/// <param name="y_offset"></param>
		void OnMouseScroll(float y_offset);
		/// <summary>
		/// ��direction����Front, ������Right��Up
		/// </summary>
		/// <param name="direction"></param>
		void UpdateFront(glm::vec3& direction);
		/// <summary>
		/// ��view��projection��䵽Uniform���������
		/// </summary>
		void FillUniformMatrices();

		UniformBuffer uniform_matrices;

	private:
		
};
#endif // !CAMERA_H



