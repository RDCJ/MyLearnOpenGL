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

void ShaderProgram::Apply(Camera& camera, bool skybox)
{
	// mvp
	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();
	if (skybox)
		// 移除观察矩阵中的位移部分，让移动不会影响天空盒的位置向量
		view = glm::mat4(glm::mat3(view));

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

void ShaderProgram::Apply(Transform& transform)
{
	auto model = transform.GetModel();
	this->SetUniformMat4f("model", model);

	// 计算法线矩阵，用于把法向量转换为世界空间坐标
	// 法线应该只受缩放和旋转变换的影响，而不受位移影响
	// 不等比缩放会导致法向量不再垂直于对应的表面
	// 因此不能直接用模型矩阵对法向量做变换，而是使用一个为法向量专门定制的模型矩阵。这个矩阵称之为法线矩阵：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
	glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
	this->SetUniformMat3f("NormalMatrix", normal_matrix);
}

void ShaderProgram::Apply(TextureCubeMap& cube_map)
{
	this->SetUniformBool("use_cube_map", true);

	this->SetUniformInt("cube_map", 0);
	glActiveTexture(GL_TEXTURE0);
	cube_map.Bind();
}

void ShaderProgram::Apply(Material& material)
{
	if (material.cube_map != nullptr)
	{
		this->SetUniformInt("cube_map", 0);
		glActiveTexture(GL_TEXTURE0);
		material.cube_map->Bind();
	}

	// 绑定纹理
	std::map<std::string, unsigned int> texture_count;
	for (std::string texture_type : Texture2D::TextureTypes) texture_count[texture_type] = 0;

	for (int i = 0; i < material.textures.size(); i++)
	{
		std::string& type = material.textures[i].type;
		unsigned int idx = texture_count[type];
		// 纹理命名规则：material.texture_{type}{idx}
		this->SetUniformInt("material." + type + std::to_string(idx), i + 1);
		texture_count[type] = idx + 1;
		// 在绑定之前激活相应的纹理单元
		glActiveTexture(GL_TEXTURE0 + i + 1);
		material.textures[i].Bind();
	}

	this->SetUniformFloat("material.shininess", material.shininess);
	this->SetUniformFloat("material.refract_ratio", material.refract_ratio);
}


