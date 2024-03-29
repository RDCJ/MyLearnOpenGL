#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vert_source_path, const char* frag_source_path)
{
	ID = glCreateProgram();
	Shader vertex_shader = Shader(vert_source_path, ShaderType::Vertex);
	Shader frag_shader = Shader(frag_source_path, ShaderType::Fragment);

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