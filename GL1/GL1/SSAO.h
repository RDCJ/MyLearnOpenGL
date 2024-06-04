#pragma once
#include "FrameBuffer.h"
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>
#include "ShaderProgram.h"

class SSAO
{
	public:
		/// <summary>
		/// ���淨�߷���������ڵ�����
		/// </summary>
		static std::vector<glm::vec3> Kernel;
		/// <summary>
		/// �������ת��
		/// </summary>
		static Texture2D* Noise;

		static ShaderProgram* shader;
		static ShaderProgram* blur_shader;
		static void Init();

		SSAO(int width, int height);

		FrameBuffer SSAOFrameBuffer;
		FrameBuffer SSAOBlurBuffer;

	private:
		static void InitSSAOKernel();
		static void InitSSAONoise();
};

