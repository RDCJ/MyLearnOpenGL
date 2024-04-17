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
		/// ��ƽ��
		/// </summary>
		float Z_Near;
		/// <summary>
		/// Զƽ��
		/// </summary>
		float Z_Far;

		Camera(float _zNear, float _zFar);

		/// <summary>
		/// ���ؿ��ŵ�ǰ����Ĺ۲���� (���ݵ�ǰ�������λ�ã��������)
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView() ;
		/// <summary>
		/// �����������Ŀ��λ�ã������ع۲����
		/// </summary>
		/// <param name="target">Ŀ��λ��</param>
		/// <returns></returns>
		glm::mat4 GetView(const glm::vec3& target);
		/// <summary>
		/// ����ͶӰ����
		/// </summary>
		/// <returns></returns>
		virtual glm::mat4 GetProjection() { return glm::mat4(1.0f); }
		/// <summary>
		/// ��direction����Front, ������Right��Up
		/// </summary>
		/// <param name="direction"></param>
		void UpdateFront(const glm::vec3& direction);
		/// <summary>
		/// ��view��projection��䵽Uniform���������
		/// </summary>
		void FillUniformMatrices();

		UniformBuffer uniform_matrices;

	private:
		
};
#endif // !CAMERA_H



