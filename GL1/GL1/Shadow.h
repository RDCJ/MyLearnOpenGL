#pragma once

#include "FrameBuffer.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include "PerspectiveCamera.h"
#include "OrthoCamera.h"
#include "Light.h"

class Shadow
{
	public:
		Light* light;
		Camera* camera = nullptr;
		FrameBuffer depth_map_buffer;
		Shadow(int width, int height, Light* light);
		virtual void Clear();
	protected:
		int width;
		int height;
		virtual void InitTexture(TexParams& depth_tex_params) = 0;
		~Shadow() 
		{
			delete camera;
		}
};

