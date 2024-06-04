#include "SSAO.h"
#include <vector>
#include <random>

std::vector<glm::vec3> SSAO::Kernel;
Texture2D* SSAO::Noise;
ShaderProgram* SSAO::shader;
ShaderProgram* SSAO::blur_shader = nullptr;

SSAO::SSAO(int width, int height): SSAOFrameBuffer(width, height), SSAOBlurBuffer(width, height)
{
	if (Kernel.size() <= 0)
	{
		SSAO::Init();
	}
	TexParams params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST}
	};
	SSAOFrameBuffer.AddTexture2D(GL_RED, GL_RGB, GL_FLOAT, 0, params);
	SSAOBlurBuffer.AddTexture2D(GL_RED, GL_RGB, GL_FLOAT, 0, params);
}

inline void SSAO::Init()
{
	InitSSAOKernel();
	InitSSAONoise();
	SSAO::shader = new ShaderProgram("./Shader/simple.vert", "./Shader/SSAO.frag");
	SSAO::blur_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/SSAO_Blur.frag");
}

inline void SSAO::InitSSAOKernel()
{
	std::uniform_real_distribution<float> randomFloats(0, 1.0);
	std::default_random_engine generator;
	Kernel.reserve(64);
	for (int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = (float)i / 64.0;
		scale = Utils::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		Kernel.push_back(sample);
	}
}

void SSAO::InitSSAONoise()
{
	std::uniform_real_distribution<float> randomFloats(0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec3> noise_data;
	noise_data.reserve(16);
	for (int i = 0; i < 16; i++)
	{
		noise_data.emplace_back(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f
		);
	}
	TexParams params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST},
		{GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}
	};
	Noise = new Texture2D(4, 4, GL_RGB16F, GL_RGB, noise_data.data(), GL_FLOAT, params);
}
