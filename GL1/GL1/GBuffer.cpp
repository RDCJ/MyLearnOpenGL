#include "GBuffer.h"

GBuffer::GBuffer(int buffer_width, int buffer_height): FrameBuffer(buffer_width, buffer_height)
{
	TexParams params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST}
	};
	gPosition = Texture2D(buffer_width, buffer_height, GL_RGB16F, GL_RGB, NULL, GL_FLOAT, params);
	gNormal = Texture2D(buffer_width, buffer_height, GL_RGB16F, GL_RGB, NULL, GL_FLOAT, params);
	gDiffuse = Texture2D(buffer_width, buffer_height, GL_RGBA, GL_RGBA, NULL, GL_FLOAT, params);
	gSpecular = Texture2D(buffer_width, buffer_height, GL_RGBA, GL_RGBA, NULL, GL_FLOAT, params);

	BindSelf();
	BindTexture2D(gPosition, GL_COLOR_ATTACHMENT0, 0);
	BindTexture2D(gNormal, GL_COLOR_ATTACHMENT1, 0);
	BindTexture2D(gDiffuse, GL_COLOR_ATTACHMENT2, 0);
	BindTexture2D(gSpecular, GL_COLOR_ATTACHMENT3, 0);

	//GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	//glDrawBuffers(3, attachments);
	AddRenderBuffer();
	UnbindSelf();
}
