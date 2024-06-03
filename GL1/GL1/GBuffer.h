#pragma once
#include "FrameBuffer.h"
class GBuffer: public FrameBuffer
{
	public:
		GBuffer(int buffer_width, int buffer_height);
		/// <summary>
		/// λ�û���
		/// </summary>
		Texture2D gPosition;
		/// <summary>
		/// ���߻���
		/// </summary>
		Texture2D gNormal;
		/// <summary>
		/// ��ɫ����
		/// </summary>
		Texture2D gDiffuse;
		/// <summary>
		///  ���滺��
		/// </summary>
		Texture2D gSpecular;
};

