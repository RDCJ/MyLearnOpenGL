#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vert_source_path, const char* frag_source_path)
{
	ID = glCreateProgram();
	Shader vertex_shader = Shader(vert_source_path, ShaderType::VertexShader);
	Shader frag_shader = Shader(frag_source_path, ShaderType::FragmentShader);

	bool f1 = vertex_shader.CheckCompileSuccess();
	bool f2 = frag_shader.CheckCompileSuccess();

	if (f1 && f2)
	{
		AttachShader(vertex_shader);
		AttachShader(frag_shader);
		Link();
		CheckLinkSuccess();
	}
}

bool ShaderProgram::CheckLinkSuccess()
{
	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		// 用glGetShaderInfoLog获取错误消息，然后打印
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
	return success;
}

void ShaderProgram::Apply(Camera& camera)
{
	// mvp
	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();

	this->SetUniformVec3("viewPos", camera.position);
	this->SetUniformMat4f("view", view);
	this->SetUniformMat4f("projection", projection);
}

void ShaderProgram::Apply(std::vector<Light*>& lights)
{
	this->SetUniformInt("light_num", lights.size());
	for (int i = 0; i < lights.size(); i++)
	{
		Light* light = lights[i];

		std::string index = "lights[" + std::to_string(i) + "]";
		this->SetUniformInt(index + ".type", (int)light->type);
		this->SetUniformVec3(index + ".position", light->position);
		this->SetUniformVec3(index + ".direction", light->direction);
		this->SetUniformFloat(index + ".innerCutOff", glm::cos(glm::radians(light->innerCutOff)));
		this->SetUniformFloat(index + ".outerCutOff", glm::cos(glm::radians(light->outerCutOff)));

		this->SetUniformVec3(index + ".ambient", light->ambient);
		this->SetUniformVec3(index + ".diffuse", light->diffuse);
		this->SetUniformVec3(index + ".specular", light->specular);

		this->SetUniformFloat(index + ".constant", light->type == LightType::Point ? light->constant : 1);
		this->SetUniformFloat(index + ".linear", light->type == LightType::Point ? light->linear : 0);
		this->SetUniformFloat(index + ".quadratic", light->type == LightType::Point ? light->quadratic : 0);
	}

}
