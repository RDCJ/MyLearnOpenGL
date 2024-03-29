#include "Shader.h"


Shader::Shader(const char** shader_source, ShaderType _shaderType)
{
	shaderType = _shaderType;
	Compile(shader_source);
}

Shader::Shader(const char* source_path, ShaderType _shaderType)
{
	shaderType = _shaderType;

	std::string source_code;
	std::ifstream shader_file;

	shader_file.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try
	{
		shader_file.open(source_path);
		std::stringstream shader_stream;
		shader_stream << shader_file.rdbuf();
		shader_file.close();
		source_code = shader_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* source_code_cstr = source_code.c_str();
	Compile(&source_code_cstr);
}

void Shader::Compile(const char** shader_source)
{
	// 创建一个着色器对象，注意是用ID来引用的
	switch (shaderType)
	{
	case ShaderType::Vertex:
		ID = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ShaderType::Fragment:
		ID = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	}
	// 把着色器源码附加到着色器对象上，然后编译它
	glShaderSource(ID, 1, shader_source, NULL);
	glCompileShader(ID);
}

bool Shader::CheckCompileSuccess()
{
	int success;
	// 用glGetShaderiv检查是否编译成功
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		// 用glGetShaderInfoLog获取错误消息，然后打印
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return success;
}
