#pragma once
#include "FrameBuffer.h"
class GBuffer: public FrameBuffer
{
	public:
		GBuffer(int buffer_width, int buffer_height);
		/// <summary>
		/// 位置缓冲
		/// </summary>
		Texture2D gPosition;
		/// <summary>
		/// 法线缓冲
		/// </summary>
		Texture2D gNormal;
		/// <summary>
		/// 颜色缓冲
		/// </summary>
		Texture2D gDiffuse;
		/// <summary>
		///  镜面缓冲
		/// </summary>
		Texture2D gSpecular;
		/// <summary>
		/// 观察空间下的位置缓冲
		/// </summary>
		Texture2D gViewPosition;
		/// <summary>
		/// 观察空间下的法线缓冲
		/// </summary>
		Texture2D gViewNormal;

};

