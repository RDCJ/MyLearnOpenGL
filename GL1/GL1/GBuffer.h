#pragma once
#include "FrameBuffer.h"
class GBuffer: public FrameBuffer
{
	public:
		GBuffer(int buffer_width, int buffer_height);
		/// <summary>
		/// Î»ÖÃ»º³å
		/// </summary>
		Texture2D gPosition;
		/// <summary>
		/// ·¨Ïß»º³å
		/// </summary>
		Texture2D gNormal;
		/// <summary>
		/// ÑÕÉ«»º³å
		/// </summary>
		Texture2D gDiffuse;
		/// <summary>
		///  ¾µÃæ»º³å
		/// </summary>
		Texture2D gSpecular;
};

