#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Transform.h"
#include "Material.h"
#include <GLFW/glfw3.h>

/// <summary>
/// ��ɫ���������(Shader Program Object)�Ƕ����ɫ���ϲ�֮������������ɵİ汾��
/// ���Ҫʹ�øղű������ɫ�����Ǳ������������(Link)Ϊһ����ɫ���������Ȼ������Ⱦ�����ʱ�򼤻������ɫ������
/// �Ѽ�����ɫ���������ɫ���������Ƿ�����Ⱦ���õ�ʱ��ʹ�á�
/// </summary>
class ShaderProgram
{
	public:
		unsigned int GetID() { return ID; }
		ShaderProgram()
		{
			// glCreateProgram��������һ�����򣬲������´�����������ID����
			ID = glCreateProgram();
		}

		ShaderProgram(const char* vert_source_path, const char* frag_source_path);

		ShaderProgram(const char* vert_source_path, const char* geometry_source_path, const char* frag_source_path);

		~ShaderProgram()
		{
			glDeleteProgram(ID);
		}

		/// <summary>
		/// ��ɫ�����ӵ����������
		/// </summary>
		/// <param name="shader"></param>
		void AttachShader(const Shader &shader)
		{
			glAttachShader(ID, shader.GetID());
		}

		/// <summary>
		/// ����
		/// </summary>
		void Link()
		{
			glLinkProgram(ID);
		}

		bool CheckLinkSuccess();

		/// <summary>
		/// ����������
		/// </summary>
		void Use()
		{
			glUseProgram(ID);
		}
		/// <summary>
		/// ���������
		/// uniform��ֵ��viewPos��view��projection
		/// </summary>
		/// <param name="camera"></param>
		void Apply(Camera& camera, bool use_uniform_matrices=false, bool skybox=false);
		/// <summary>
		/// ���ù���
		/// </summary>
		/// <param name="lights"></param>
		void Apply(std::vector<Light*>& lights);
		/// <summary>
		/// ����model��NormalMatrix
		/// </summary>
		/// <param name="transform"></param>
		void Apply(Transform& transform);
		/// <summary>
		/// ���ò���
		/// </summary>
		/// <param name="material"></param>
		void Apply(Material& material);
		/// <summary>
		/// ����һ�����ڻ���ӳ�����������ͼ
		/// </summary>
		/// <param name="cube_map"></param>
		void Apply(TextureCubeMap& cube_map);
		/// <summary>
		/// ����uniform��İ󶨵�
		/// </summary>
		/// <param name="uniform_buffer"></param>
		void Apply(UniformBuffer& uniform_buffer);

#pragma region Set Uniform
		void SetUniformBool(const std::string& name, bool value) const
		{
			glUniform1i(GetUniformLocation(name.c_str()), (int)value);
		}

		void SetUniformInt(const std::string& name, int value) const
		{
			glUniform1i(GetUniformLocation(name.c_str()), value);
		}

		void SetUniformFloat(const std::string& name, float value) const
		{
			glUniform1f(GetUniformLocation(name.c_str()), value);
		}

		void SetUniformMat3f(const std::string& name, glm::mat3& mat, bool transpose = false) const
		{
			//�ڶ�����������OpenGL���ǽ�Ҫ���Ͷ��ٸ�����
			// ������������ʾ�Ƿ�Ծ������ת��(Transpose)��OpenGLͨ��ʹ��������(Column-major Ordering)���֡�GLM��Ĭ�ϲ��־�������������ͨ��������Ҫת�þ���
			// ���һ�������������ľ������ݣ�����GLM�����ǰ����ǵľ��󴢴�ΪOpenGL��ϣ�����ܵ����֣��������Ҫ����GLM���Դ��ĺ���value_ptr���任��Щ����
			glUniformMatrix3fv(GetUniformLocation(name.c_str()), 1, (int)transpose, glm::value_ptr(mat));
		}

		void SetUniformMat4f(const std::string& name, glm::mat4& mat, bool transpose = false) const
		{
			//�ڶ�����������OpenGL���ǽ�Ҫ���Ͷ��ٸ�����
			// ������������ʾ�Ƿ�Ծ������ת��(Transpose)��OpenGLͨ��ʹ��������(Column-major Ordering)���֡�GLM��Ĭ�ϲ��־�������������ͨ��������Ҫת�þ���
			// ���һ�������������ľ������ݣ�����GLM�����ǰ����ǵľ��󴢴�ΪOpenGL��ϣ�����ܵ����֣��������Ҫ����GLM���Դ��ĺ���value_ptr���任��Щ����
			glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, (int)transpose, glm::value_ptr(mat));
		}

		void SetUniformVec3(const std::string& name, glm::vec3 vec) const
		{
			glUniform3f(GetUniformLocation(name.c_str()), vec.x, vec.y, vec.z);
		}

		void SetUniformVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(GetUniformLocation(name.c_str()), x, y, z);
		}
#pragma endregion

		int GetUniformLocation(const char* name) const
		{
			return glGetUniformLocation(ID, name);
		}

	private:
		unsigned int ID;
};