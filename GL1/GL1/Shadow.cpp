#include "Shadow.h"

Shadow::Shadow(int width, int height, Light* light):width(width), height(height), light(light), depth_map_buffer(width, height)
{

}

void Shadow::Clear()
{
	depth_map_buffer.UpdateViewport();
	depth_map_buffer.Bind();
	// 绑定帧缓冲后要清除之前的缓存
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
