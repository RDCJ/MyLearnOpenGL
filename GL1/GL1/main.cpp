//��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���
//GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL / gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD

#include <iostream>
#include <cmath>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "stb_image.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Time.h"
#include "Light.h"
#include "Mesh.h"
#include "Model.h"
#include "Transform.h"
#include "Object.h"
#include "TextureCubeMap.h"

const int ScreenWidth = 1600;
const int ScreenHeight = 1200;

Camera* camera = nullptr;
bool firstMouse = true;
glm::vec2 mouseLastPos = 0.5f * glm::vec2(ScreenWidth, ScreenHeight);

void InitGLFW()
{
	glfwInit();
	// ʹ��glfwWindowHint����������GLFW

	// ���汾��(Major)�ʹΰ汾��(Minor)����Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// ����GLFWʹ�ú���ģʽ(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool InitGLAD()
{
	int error_code = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (error_code != 1)
		std::cout << "Failed to init GLAD, error code: "<< error_code << std::endl;
	return error_code == 1;
}

/// <summary>
/// ���û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ�����
/// </summary>
/// <param name="window"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// glViewport����ǰ�����������ƴ������½ǵ�λ�á��������͵��ĸ�����������Ⱦ���ڵĿ�Ⱥ͸߶ȣ����أ�
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	// glfwGetKey��������Ҫһ�������Լ�һ��������Ϊ���롣����������᷵����������Ƿ����ڱ�����
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// glfwSetwindowShouldClose��WindowShouldClose��������Ϊtrue���ر�GLFW
		glfwSetWindowShouldClose(window, true);
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		firstMouse = false;
		mouseLastPos = glm::vec2(xpos, ypos);
	}
	float xoffset = xpos - mouseLastPos.x;
	float yoffset = mouseLastPos.y - ypos;
	mouseLastPos.x = xpos;
	mouseLastPos.y = ypos;
	camera->OnMouseMove(xoffset, yoffset);
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->OnMouseScroll(yoffset);
}

static std::vector<Light*> CreateLight()
{
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	std::vector<Light*> lights = std::vector<Light*>();
	for (int i = 0; i < 4; i++)
	{
		Light* light = Light::CreatePoint(pointLightPositions[i], 1, 0.09f, 0.032f);
		lights.push_back(light);
	}

	Light* spot_light = Light::CreateSpot(glm::vec3(0, 2, 0), glm::vec3(-0.2f, -1.0f, -0.3f), 12.5f, 20.0f);
	Light* directional_light = Light::CreateDirectional(glm::vec3(0, 2, 0), glm::vec3(-1, -1, -1));

	lights.push_back(directional_light);
	//lights.push_back(spot_light);

	for (Light* light : lights)
	{
		light->ambient = glm::vec3(0.2f);
		light->diffuse = glm::vec3(1);
		light->specular = glm::vec3(1);
	}

	return lights;
	//directional_light->diffuse = glm::vec3(1.0, 1.0, 0);
}

int main() 
{
	InitGLFW();
	
	// glfwCreateWindow������Ҫ���ڵĿ�͸���Ϊ����ǰ����������������������ʾ������ڵ����ƣ����⣩
	GLFWwindow* window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Learn", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ֪ͨGLFW�����Ǵ��ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);

	// ����GLFW����Ӧ�����ع�꣬����׽(Capture)��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ע�ắ��������GLFW����ϣ��ÿ�����ڵ�����С��ʱ������������
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// ע��ص����������һ�ƶ�MouseCallback�����ͻᱻ����
	glfwSetCursorPosCallback(window, MouseMoveCallback);

	// ע��ص�������������
	glfwSetScrollCallback(window, MouseScrollCallback);

	// glad��ʼ��Ҫ������opengl context֮��ִ��
	if (!InitGLAD()) return -1;

	ShaderProgram* light_shaderProgram = new ShaderProgram("./Shader/LightVert.vert", "./Shader/LightFrag.frag");
	ShaderProgram* phong_shader = new ShaderProgram("./Shader/shader.vert", "./Shader/phong_shader.frag");
	ShaderProgram* outline_shader = new ShaderProgram("./Shader/shader.vert", "./Shader/SingleColor.frag");
	ShaderProgram* blend_shader = new ShaderProgram("./Shader/shader.vert", "./Shader/blend_shader.frag");
	ShaderProgram* frame_buffer_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/simple_texture.frag");
	ShaderProgram* skybox_shader = new ShaderProgram("./Shader/skybox.vert", "./Shader/skybox.frag");


	camera = new Camera(window, 45.0f, (float)ScreenWidth / ScreenHeight);

	Model nanosuit("./Model/nanosuit/nanosuit.obj");

	std::vector<Vertex> square_vertices
	{
		Vertex {glm::vec3(-1, -1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
		Vertex {glm::vec3(1, -1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
		Vertex {glm::vec3(1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1)},
		Vertex {glm::vec3(-1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1)},
	};
	std::vector<unsigned int> square_indices{ 0, 1, 2, 2, 3, 0 };

#pragma region cube model
	float vertices[] = {
		// positions          // normals           // texture coords
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		   -0.5f,  0.5f, 0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	};

	std::vector<Vertex> _vertices;
	for (int i = 0; i < 36; i++)
	{
		Vertex v{
			glm::vec3(vertices[i * 8], vertices[i * 8 + 1], vertices[i * 8 + 2]),
			glm::vec3(vertices[i * 8 + 3], vertices[i * 8 + 4], vertices[i * 8 + 5]),
			glm::vec2(vertices[i * 8 + 6], vertices[i * 8 + 7])
		};
		_vertices.push_back(v);
	}
	std::vector<unsigned int> _indices;
	for (int i = 0; i < 36; i++)
	{
		_indices.push_back(i);
	}

	Texture2D diffuse_map = Texture2D("Image/container2.png", "texture_diffuse");
	diffuse_map.SetParameters();
	diffuse_map.GenerateMipmap();

	Texture2D specular_map = Texture2D("Image/container2_specular.png", "texture_specular");
	specular_map.SetParameters();
	specular_map.GenerateMipmap();

	Texture2D emission_map = Texture2D("Image/matrix.jpg", "texture_emission");
	emission_map.SetParameters();
	emission_map.GenerateMipmap();
	std::vector<Texture2D> _textures{ diffuse_map , specular_map };

	Mesh cube_mesh(_vertices, _indices, _textures);
	Mesh light_mesh(_vertices, _indices, std::vector<Texture2D>());

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
#pragma endregion

#pragma region grass model
	std::vector<Texture2D> grass_texture{ Texture2D("./Image/grass.png", "texture_diffuse") };
	std::vector<Texture2D> window_Texture{ Texture2D("./Image/red_window.png", "texture_diffuse") };

	Mesh grass_mesh(square_vertices, square_indices, grass_texture);
	Mesh window_mesh(square_vertices, square_indices, window_Texture);
#pragma endregion

#pragma region skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	
	std::vector<Vertex> skybox_vertices;
	for (int i = 0; i < 36; i++)
	{
		Vertex v{
			glm::vec3(skyboxVertices[i * 3], skyboxVertices[i * 3 + 1], skyboxVertices[i * 3 + 2]),
			glm::vec3(0), glm::vec2(0)
		};
		skybox_vertices.push_back(v);
	}

	Mesh skybox_mesh(skybox_vertices, _indices, std::vector<Texture2D>());

	std::vector<std::string> skybox_img_paths{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	for (int i = 0; i < skybox_img_paths.size(); i++)
		skybox_img_paths[i] = "./Image/skybox/" + skybox_img_paths[i];
	TextureCubeMap skybox_texture(skybox_img_paths);

#pragma endregion



	Time::Init();

	glm::vec3 cubePosition = glm::vec3(0, 0, -3);

#pragma region ���ù�Դ
	auto lights = CreateLight();
#pragma endregion
	// �������޳�
	glEnable(GL_CULL_FACE);
	// glCullFace�������������õ�ѡ�
	//GL_BACK��ֻ�޳������档
	//	GL_FRONT��ֻ�޳������档
	//	GL_FRONT_AND_BACK���޳�������ͱ�����
	glCullFace(GL_BACK);
	// glFrontFace��������Ļ���˳��
	// GL_CCW����ʱ�룬GL_CW��˳ʱ��
	// Ĭ�ϣ�GL_CCW
	glFrontFace(GL_CCW);

	// ���û��
	glEnable(GL_BLEND);
	// glBlendFunc������������������������Դ��Ŀ������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


#pragma region ����֡����
	const float frame_buffer_scale = 0.07f;
	const int frame_buffer_width = ScreenWidth * frame_buffer_scale;
	const int frame_buffer_height = ScreenHeight * frame_buffer_scale;

	unsigned int frame_buffer;
	// ����һ��֡�������
	glGenFramebuffers(1, &frame_buffer);
	// glBindFramebuffer: ��֡����, �󶨵�GL_FRAMEBUFFERĿ��֮�����еĶ�ȡ��д��֡����Ĳ�������Ӱ�쵱ǰ�󶨵�֡����
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	// ��֡���崴��������ʱ������������ѡ��������Ⱦ�������

	// ��������
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	// ���Ǹ������data����������NULL, ��������������ǽ����������ڴ��û�����������������������������Ⱦ��֡����֮��������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_buffer_width, frame_buffer_height, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// ���ڶ���Ļ����ı�Ե���в�����ʱ�����ڻ��������������Χ��8�����ؽ��в�������ʵ��ȡ������֮������ء�
	// ���ڻ��Ʒ�ʽĬ����GL_REPEAT��������û�����õ������ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬��Ϳ��ܻ�����Ļ��Ե��������ֵ����ơ�
	// Ϊ��������һ���⣬���ǿ��Խ���Ļ����Ļ��Ʒ�ʽ������ΪGL_CLAMP_TO_EDGE��
	// ��������ȡ�������������ʱ�����ܹ��ظ���Ե������������ȷ�ع������յ�ֵ�ˡ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	// �������ӵ�֡�����ϣ� ֮�����е���Ⱦָ���д�뵽���������
	// glFramebufferTexture2D������
	// target��֡�����Ŀ�꣨���ơ���ȡ�������߽��У�
	// attachment��������Ҫ���ӵĸ������͡���ǰ�������ڸ���һ����ɫ������ע������0��ζ�����ǿ��Ը��Ӷ����ɫ���������ǽ���֮��Ľ̳����ᵽ��
	//	textarget����ϣ�����ӵ���������
	// texture��Ҫ���ӵ�����id
	//	level��mipmap�ļ���
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// ������Ⱦ�������
	// ��Ⱦ�����������ݴ���ΪOpenGLԭ������Ⱦ��ʽ������Ϊ������Ⱦ��֡�����Ż���
	// ��д����߸����������ݵ�����������ʱ�Ƿǳ���ġ����ԣ��������������Ĳ�����ʹ����Ⱦ�������ʱ��ǳ���
	// ��Ⱦ�������ͨ������ֻд�ģ������㲻�ܶ�ȡ���ǣ�����ʹ��������ʣ�
	// ������Ⱦ�������ͨ������ֻд�ģ����ǻᾭ��������Ⱥ�ģ�帽������Ϊ�󲿷�ʱ�����Ƕ�����Ҫ����Ⱥ�ģ�建���ж�ȡֵ��ֻ������Ⱥ�ģ�����
	unsigned int rbo;
	// ����һ����Ⱦ�������
	glGenRenderbuffers(1, &rbo);
	// ����Ⱦ�������rbo��GL_RENDERBUFFER����֮�����е���Ⱦ�������Ӱ�쵱ǰ��rbo
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// ����Ⱦ�����������ڴ档
	// GL_DEPTH24_STENCIL8��Ϊ�ڲ���ʽ������װ��24λ����Ⱥ�8λ��ģ�建�壬������Ⱦ�������������Ⱥ�ģ����Ⱦ����
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frame_buffer_width, frame_buffer_height);
	// ���
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// ����Ⱦ������󸽼ӵ�֡������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// ���֡�����Ƿ��������ģ�������ǣ�����ӡ������Ϣ��
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// ���֡����
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Mesh frame_buffer_mesh(square_vertices, square_indices, std::vector<Texture2D>());
#pragma endregion

	bool use_frame_buffer = false;
	std::cout << "��ʼ��Ⱦ" << std::endl;
	// ���һ��whileѭ�������ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	// glfwWindowShouldClose����������ÿ��ѭ���Ŀ�ʼǰ���һ��GLFW�Ƿ�Ҫ���˳�������ǵĻ����ú�������true����Ⱦѭ����ֹͣ���У�֮�����ǾͿ��Թر�Ӧ�ó���
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);
		camera->Update();

		if (use_frame_buffer)
		{
			glViewport(0, 0, frame_buffer_width, frame_buffer_height);
			// ��֡����, ��֮�����ȾӰ�쵱ǰ�󶨵�֡����
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		}

		glEnable(GL_DEPTH_TEST);

		// glClearColor���������Ļ���õ���ɫ��������glClear�����������ɫ����֮��������ɫ���嶼�ᱻ���ΪglClearColor�������õ���ɫ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear���������Ļ����ɫ���壬������һ������λ(Buffer Bit)��ָ��Ҫ��յĻ��壬���ܵĻ���λ��GL_COLOR_BUFFER_BIT��GL_DEPTH_BUFFER_BIT��GL_STENCIL_BUFFER_BIT
		// ÿ����Ⱦ����֮ǰ�����Ȼ��壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor������һ��״̬���ú�������glClear��������һ��״̬ʹ�õĺ�������ʹ���˵�ǰ��״̬����ȡӦ�����Ϊ����ɫ
		//spot_light->position = camera->position;
		//spot_light->direction = camera->Front;

#pragma region box
		for (int i = 0; i < 10; i++)
		{
			// glEnable��glDisable���������������û����ĳ��OpenGL���ܡ�������ܻ�һֱ��������/����״̬��ֱ����һ������������/������
			// ������Ȳ��ԣ���Ҫ����GL_DEPTH_TEST
			glEnable(GL_DEPTH_TEST);
			// ����ģ�����
			glEnable(GL_STENCIL_TEST);
			// ���ò���ͨ����ʧ��ʱ����Ϊ
			// ��һ��������ģ�����ʧ��ʱ��ȡ����Ϊ, 
			// �ڶ���������ģ�����ͨ��������Ȳ���ʧ��ʱ��ȡ����Ϊ, 
			// ������������ģ����Ժ���Ȳ��Զ�ͨ��ʱ��ȡ����Ϊ
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			// ����ģ�庯��
			// ��һ������������ģ����Ժ���(Stencil Test Function)��������Ժ�������Ӧ�õ��Ѵ����ģ��ֵ�Ϻ�glStencilFunc������refֵ�ϡ�
			//		���õ�ѡ���У�GL_NEVER��GL_LESS��GL_LEQUAL��GL_GREATER��GL_GEQUAL��GL_EQUAL��GL_NOTEQUAL��GL_ALWAYS
			// �ڶ���������������ģ����ԵĲο�ֵ��ģ�建������ݽ��������ֵ���бȽ�
			// ����������������һ�����룬��������ο�ֵ�ʹ����ģ��ֵ�ڲ��ԱȽ�����֮ǰ������(AND)���㡣��ʼ���������λ��Ϊ1
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			// ����ģ�建�����ǰ�����룬�����뽫Ҫд�뻺���ģ��ֵ������(AND)����
			glStencilMask(0xFF);

			glm::mat4 model = glm::mat4(1.0f); // ͨ���������������ģ�;������ǽ��ö�������任����������
			model = glm::translate(model, cubePositions[i]);

			float angle = 29 * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			// ����һ��uniform֮ǰ�������ʹ��shader���򣨵���glUseProgram)����Ϊ�����ڵ�ǰ�������ɫ������������uniform��
			phong_shader->Use();
			phong_shader->SetUniformMat4f("model", model);
			phong_shader->Apply(*camera);
			phong_shader->SetUniformFloat("material.shininess", 0.4f * 128);
			phong_shader->Apply(lights);

			// ���㷨�߾������ڰѷ�����ת��Ϊ����ռ�����
			// ����Ӧ��ֻ�����ź���ת�任��Ӱ�죬������λ��
			// ���ȱ����Żᵼ�·��������ٴ�ֱ�ڶ�Ӧ�ı���
			// ��˲���ֱ����ģ�;���Է��������任������ʹ��һ��Ϊ������ר�Ŷ��Ƶ�ģ�;�����������֮Ϊ���߾���ģ�;������Ͻ�3x3���ֵ�������ת�þ���
			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
			phong_shader->SetUniformMat3f("NormalMatrix", normal_matrix);

			////glDrawArrays������һ�����������Ǵ�����Ƶ�OpenGLͼԪ�����͡��ڶ�������ָ���˶����������ʼ���������һ������ָ�����Ǵ�����ƶ��ٸ�����
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*phong_shader);

			// ʹ��GL_ALWAYSģ����Ժ��������Ǳ�֤��Ƭ����Զ��ͨ��ģ����ԣ��ڻ���Ƭ�εĵط���ģ�建��ᱻ����Ϊ�ο�ֵ1
			// ģ�庯������ΪGL_NOTEQUAL����ǰģ�建��ֵ��Ϊ1��ƬԪ����ͨ��ģ�����
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			// ������ֵ��Ϊ0�� ��ֹģ�建�����
			glStencilMask(0x00);
			//glDisable(GL_DEPTH_TEST);

			model = glm::scale(model, glm::vec3(1.05));
			outline_shader->Use();
			outline_shader->SetUniformMat4f("model", model);
			outline_shader->Apply(*camera);
			outline_shader->SetUniformVec3("color", glm::vec3(0, 0.1f * (i + 1), 0));
			cube_mesh.Draw(*outline_shader);

			// glStencilMask(0x00)��������ֹģ�建���д�룬Ҳ����ֹ�����(glClear(stencil_buffer)��Ч)
			// �����Ҫ����Ϊ0xFF
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			// �ر�ģ�����
			glDisable(GL_STENCIL_TEST);
		}
#pragma endregion
		
#pragma region nanosuit

		Transform transform(glm::vec3(0, 0, 1), glm::vec3(0.1f));

		phong_shader->Use();
		phong_shader->Apply(*camera);
		phong_shader->SetUniformFloat("material.shininess", 0.4f * 128);
		phong_shader->Apply(lights);
		phong_shader->Apply(transform);

		nanosuit.Draw(*phong_shader);
#pragma endregion

#pragma region  light box
		for (int i = 0; i < lights.size() - 1; i++)
		{
			Light* light = lights[i];

			light_shaderProgram->Use();
			light_shaderProgram->Apply(*camera);
			light_shaderProgram->SetUniformVec3("lightColor", light->diffuse);

			transform = Transform(light->position, glm::vec3(0.2f));
			light_shaderProgram->Apply(transform);

			light_mesh.Draw(*light_shaderProgram);
		}
#pragma endregion

#pragma region skybox
		// ��պе�����ڶ�����ɫ���б�ǿ����Ϊ1������Ȼ����Ĭ��ֵҲΪ1
		// ����������Ҫ��֤��պ���ֵС�ڻ������Ȼ��������С��ʱͨ����Ȳ���
		glDepthFunc(GL_LEQUAL);
		skybox_shader->Use();
		skybox_shader->SetUniformInt("cube_map", 0);
		skybox_shader->Apply(*camera, true);
		glActiveTexture(GL_TEXTURE0);
		skybox_texture.Bind();
		skybox_mesh.Draw(*skybox_shader);
		glDepthFunc(GL_LESS);
#pragma endregion

#pragma region ��͸���ȵ�����
		glDisable(GL_CULL_FACE);
		std::vector<Object> objs(3);

		transform = Transform(glm::vec3(0, 0, 2), glm::vec3(1.0f));
		objs.emplace(objs.end(), &grass_mesh, transform);

		for (int i = 0; i < 2; i++)
		{
			transform = Transform(glm::vec3(0, 0, 4- i), glm::vec3(1.0f));
			objs.emplace(objs.end(), &window_mesh, transform);
		}

		std::map<float, Object*> sorted;
		for (int i = 0; i < objs.size(); i++)
		{
			float dist = glm::length(objs[i].transform.position - camera->position);
			sorted[dist] = &objs[i];
		}
		
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			blend_shader->Use();
			blend_shader->Apply(*camera);
			blend_shader->Apply(it->second->transform);
			it->second->Draw(*blend_shader);
		}
#pragma endregion

		if (use_frame_buffer)
		{
			glViewport(0, 0, ScreenWidth, ScreenHeight);
			// ��ʱ��������Ⱦ����Ѿ������֡����ĸ�����������
			// ���֡����
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			// �����Ļ����ɫ�ͻ���
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// ������������Ϊ��ͼ����Ⱦһ���ı���
			frame_buffer_shader->Use();
			frame_buffer_shader->SetUniformInt("tex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			frame_buffer_mesh.Draw(*frame_buffer_shader);
		}

		// glfwSwapBuffers�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壩��������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ��
		glfwSwapBuffers(window);
		// glfwPollEvents���������û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����
		glfwPollEvents();
	}

	// ����Ⱦѭ��������������Ҫ��ȷ�ͷ�/ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}