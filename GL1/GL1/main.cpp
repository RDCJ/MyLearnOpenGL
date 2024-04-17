//��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���
//GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL / gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD

#include <iostream>
#include <cmath>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "Time.h"
#include "Model.h"
#include "Object.h"
#include "FrameBuffer.h"
#include "FPSController.h"
#include "OrthoCamera.h"

const int ScreenWidth = 1600;
const int ScreenHeight = 1200;

Camera* camera = nullptr;
FPSController* fps_crtl = nullptr;

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
	fps_crtl->OnMouseMove(xoffset, yoffset);
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fps_crtl->OnMouseScroll(yoffset);
}

static std::vector<Light*> CreateLight()
{
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.0f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	std::vector<Light*> lights = std::vector<Light*>();
	//for (int i = 0; i < 4; i++)
	//{
	//	Light* light = Light::CreatePoint(pointLightPositions[i], 1, 0.09f, 0.032f);
	//	lights.push_back(light);
	//}

	Light* spot_light = Light::CreateSpot(glm::vec3(0, 2, 0), glm::vec3(-0.2f, -1.0f, -0.3f), 12.5f, 20.0f);
	Light* directional_light = Light::CreateDirectional(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(2.0f, -4.0f, 1.0f));

	lights.push_back(directional_light);
	//lights.push_back(spot_light);

	for (Light* light : lights)
	{
		light->ambient = glm::vec3(0.2f);
		light->diffuse = glm::vec3(1);
		light->specular = glm::vec3(1);
	}
	directional_light->diffuse = glm::vec3(0.5);
	return lights;
	//directional_light->diffuse = glm::vec3(1.0, 1.0, 0);
}

int main() 
{
	InitGLFW();

	// �ڴ�������֮ǰ����glfwWindowHint, ʹ��һ������N�������Ķ��ز�������
	glfwWindowHint(GLFW_SAMPLES, 4);
	
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
	ShaderProgram* phong_shader = new ShaderProgram("./Shader/MVP.vert", "./Shader/Blinn_Phong.frag");
	ShaderProgram* outline_shader = new ShaderProgram("./Shader/MVP.vert", "./Shader/SingleColor.frag");
	ShaderProgram* blend_shader = new ShaderProgram("./Shader/MVP.vert", "./Shader/blend_shader.frag");
	ShaderProgram* frame_buffer_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/simple_texture.frag");
	ShaderProgram* skybox_shader = new ShaderProgram("./Shader/skybox.vert", "./Shader/skybox.frag");
	ShaderProgram* explode_shader = new ShaderProgram("./Shader/explode.vert", "./Shader/explode.geo", "./Shader/phong_shader_with_geo.frag");
	ShaderProgram* draw_normal_shader = new ShaderProgram("./Shader/draw_normal.vert", "./Shader/draw_normal.geo", "./Shader/SingleColor.frag");
	ShaderProgram* phong_instance_shader = new ShaderProgram("./Shader/MVP_instance.vert", "./Shader/Blinn_Phong.frag");
	ShaderProgram* phong_instance_array_shader = new ShaderProgram("./Shader/MVP_instance_array.vert", "./Shader/Blinn_Phong.frag");
	ShaderProgram* depth_shader = new ShaderProgram("./Shader/SimpleDepthShader.vert", "./Shader/Empty.frag");
	ShaderProgram* depth_texture_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/Depth.frag");

	Material empty_material;

	camera = new OrthoCamera(-10, 10, -10, 10, 0.1, 100);
	//camera = new PerspectiveCamera(45.0f, (float)ScreenWidth / ScreenHeight);
	fps_crtl = new FPSController(camera, window);
	// ��camera����һ��uniform�������
	camera->uniform_matrices.GenBuffer(2 * sizeof(glm::mat4), 0);

	Model nanosuit("./Model/nanosuit_reflection/nanosuit.obj");
	for (int i = 0; i < nanosuit.materials.size(); i++)
		nanosuit.materials[i].shininess = 0.4f * 128;

	std::vector<Vertex> square_vertices
	{
		Vertex {glm::vec3(-1, -1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
		Vertex {glm::vec3(1, -1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
		Vertex {glm::vec3(1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1)},
		Vertex {glm::vec3(-1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1)},
	};
	std::vector<unsigned int> square_indices{ 0, 1, 2, 2, 3, 0 };

#pragma region skybox_texture
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
	diffuse_map.GenerateMipmap();

	Texture2D specular_map = Texture2D("Image/container2_specular.png", "texture_specular");
	specular_map.GenerateMipmap();

	Texture2D emission_map = Texture2D("Image/matrix.jpg", "texture_emission");
	emission_map.GenerateMipmap();

	std::vector<Texture2D> _textures{ diffuse_map , specular_map };

	Material cube_material(_textures, 0.4f * 128, 1.0 / 1.52);
	Mesh cube_mesh(_vertices, _indices);
	Mesh light_mesh(_vertices, _indices);

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

	Material grass_material(grass_texture);
	Material window_material(window_Texture);

	Mesh grass_mesh(square_vertices, square_indices);
	Mesh window_mesh(square_vertices, square_indices);
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

	Material skybox_material(&skybox_texture);
	Mesh skybox_mesh(skybox_vertices, _indices);
#pragma endregion

#pragma region box instance model matrix
	int box_instance_num = 100;

	std::vector<glm::mat4> box_instance_model;
	for (int i = 0; i < box_instance_num; i++)
	{
		Transform tf;
		tf.position = glm::vec3(0.05 * i * std::sin(i), 0.05 * i * std::cos(i), 0.05 * i * std::cos(i) * std::sin(i)) + glm::vec3(0, 0, 10);
		tf.scale = glm::vec3(0.15);
		box_instance_model.push_back(tf.GetModel());
	}
#pragma endregion

#pragma region planet
	bool planet_use_instance = true;
	
	float planet_size = 2;
	int rock_num = 10000;
	float rock_radius_base = 50;
	float rock_radius_param = 0.1;
	float rock_vertical_param = 4;

	float rock_min_size = 0.05, rock_max_size = 0.1;
	float rock_around_speed = 0.02f;

	srand(1024);
	
	Model planet("./Model/planet/planet.obj", true);
	for (int i = 0; i < planet.materials.size(); i++)
		planet.materials[i].shininess = 0.1f * 128;
	Transform planet_tf(glm::vec3(rock_radius_base + 10, 0, 0), glm::vec3(planet_size));

	Model rock("./Model/rock/rock.obj");
	for (int i = 0; i < rock.materials.size(); i++)
		rock.materials[i].shininess = 0.1f * 128;

	std::vector<float> rock_radius(rock_num);
	std::vector<float> rock_angle(rock_num);
	std::vector<float> rock_y(rock_num);


	std::vector<Transform> rock_tfs(rock_num);
	std::vector<glm::mat4> rock_models(rock_num);
	for (int i = 0; i < rock_num; i++)
	{
		rock_radius[i] = rock_radius_base + rock_radius_param * Utils::RandomFloat(-rock_radius_base, rock_radius_base);

		rock_angle[i] = (float)i / rock_num * 360 + Utils::RandomFloat(-1, 1);
		rock_y[i] = rock_vertical_param * Utils::RandomFloat(-rock_max_size, rock_max_size);

		glm::vec3 pos = planet_tf.position + 
									glm::vec3(
										rock_radius[i] * std::sin(rock_angle[i]),
										rock_y[i],
										rock_radius[i] * std::cos(rock_angle[i]));
		glm::vec3 scale(Utils::RandomFloat(rock_min_size, rock_max_size));
		glm::vec3 rotate_axis(Utils::RandomFloat(0, 1));

		rock_tfs[i] = Transform(pos, scale, rotate_axis, Utils::RandomInt(0, 360));

		rock_models[i] = rock_tfs[i].GetModel();
	}

	unsigned int rock_model_instance_vbo;
	if (planet_use_instance)
	{
		glGenBuffers(1, &rock_model_instance_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, rock_model_instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, rock_num * sizeof(glm::mat4), rock_models.data(), GL_STATIC_DRAW);

		for (int i = 0; i < rock.meshes.size(); i++)
		{
			rock.meshes[i].SetInstanceMat4(3);
		}
	}
#pragma endregion

	Time::Init();
	
	auto lights = CreateLight();

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

	// ���ö��ز������ڴ����OpenGL�������ϣ����ز�������Ĭ������
	glEnable(GL_MULTISAMPLE);

	Mesh frame_buffer_mesh(square_vertices, square_indices);

#pragma region �����ͼ֡����
	FrameBuffer depth_map_buffer(1024, 1024);

	TexParams depth_tex_params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST},
		{GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}
	};
	// ��Ϊֻ�������ֵ���������ʽָ��ΪGL_DEPTH_COMPONENT
	depth_map_buffer.AddTexture(GL_DEPTH_COMPONENT, GL_FLOAT, 0, depth_tex_params);
#pragma endregion

	OrthoCamera shadow_camera(-10, 10, -10, 10, 1, 7.5);
	shadow_camera.position = lights[0]->position;
	shadow_camera.UpdateFront(lights[0]->direction);

	bool use_box_outline = false;
	bool active_skybox = false;
	bool use_explode = false;
	bool draw_normal = false;
	bool rock_move = true;
	bool use_blinn = true;

	std::cout << "��ʼ��Ⱦ" << std::endl;
	// ���һ��whileѭ�������ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	// glfwWindowShouldClose����������ÿ��ѭ���Ŀ�ʼǰ���һ��GLFW�Ƿ�Ҫ���˳�������ǵĻ����ú�������true����Ⱦѭ����ֹͣ���У�֮�����ǾͿ��Թر�Ӧ�ó���
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);
		fps_crtl->Update();
		camera->FillUniformMatrices();

		glEnable(GL_DEPTH_TEST);

		// glClearColor���������Ļ���õ���ɫ��������glClear�����������ɫ����֮��������ɫ���嶼�ᱻ���ΪglClearColor�������õ���ɫ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear���������Ļ����ɫ���壬������һ������λ(Buffer Bit)��ָ��Ҫ��յĻ��壬���ܵĻ���λ��GL_COLOR_BUFFER_BIT��GL_DEPTH_BUFFER_BIT��GL_STENCIL_BUFFER_BIT
		// ÿ����Ⱦ����֮ǰ�����Ȼ��壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor������һ��״̬���ú�������glClear��������һ��״̬ʹ�õĺ�������ʹ���˵�ǰ��״̬����ȡӦ�����Ϊ����ɫ
		
#pragma region �����ͼ
		depth_map_buffer.UpdateViewport();
		depth_map_buffer.Bind();
		// ��֡�����Ҫ���֮ǰ�Ļ���
		glClear(GL_DEPTH_BUFFER_BIT);
		depth_shader->Use();
		depth_shader->Apply(shadow_camera);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		for (int i = 0; i < 10; i++)
		{
			glEnable(GL_DEPTH_TEST);
			float angle = 29 * i;
			Transform tf(cubePositions[i], glm::vec3(1), glm::vec3(1.0f, 0.3f, 0.5f), angle);
			depth_shader->Apply(tf);
			cube_mesh.Draw(*depth_shader);
		}
#pragma endregion

		// ���֡����
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// ��ʱ��������Ⱦ����Ѿ������֡����ĸ�����������
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glDisable(GL_DEPTH_TEST);
		// ������������Ϊ��ͼ����Ⱦһ���ı���
		depth_texture_shader->Use();
		depth_texture_shader->SetUniformInt("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_map_buffer.color_buffer->GetID());
		frame_buffer_mesh.Draw(*depth_texture_shader);


/*
#pragma region box
		for (int i = 0; i < 10; i++)
		{
			// glEnable��glDisable���������������û����ĳ��OpenGL���ܡ�������ܻ�һֱ��������/����״̬��ֱ����һ������������/������
			// ������Ȳ��ԣ���Ҫ����GL_DEPTH_TEST
			glEnable(GL_DEPTH_TEST);

			if (use_box_outline)
			{
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
			}
			
			float angle = 29 * i;
			Transform tf(cubePositions[i], glm::vec3(1), glm::vec3(1.0f, 0.3f, 0.5f), angle);
			// ����һ��uniform֮ǰ�������ʹ��shader���򣨵���glUseProgram)����Ϊ�����ڵ�ǰ�������ɫ������������uniform��
			phong_shader->Use();
			phong_shader->Apply(tf);
			phong_shader->Apply(*camera, true);
			phong_shader->Apply(lights);
			phong_shader->SetUniformBool("use_cube_map", false);
			phong_shader->SetUniformBool("use_blinn", use_blinn);
			phong_shader->Apply(cube_material);
			////glDrawArrays������һ�����������Ǵ�����Ƶ�OpenGLͼԪ�����͡��ڶ�������ָ���˶����������ʼ���������һ������ָ�����Ǵ�����ƶ��ٸ�����
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*phong_shader);

			if (use_box_outline)
			{
				// ʹ��GL_ALWAYSģ����Ժ��������Ǳ�֤��Ƭ����Զ��ͨ��ģ����ԣ��ڻ���Ƭ�εĵط���ģ�建��ᱻ����Ϊ�ο�ֵ1
				// ģ�庯������ΪGL_NOTEQUAL����ǰģ�建��ֵ��Ϊ1��ƬԪ����ͨ��ģ�����
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				// ������ֵ��Ϊ0�� ��ֹģ�建�����
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);

				tf.scale = glm::vec3(1.05);
				outline_shader->Use();
				outline_shader->Apply(tf);
				outline_shader->Apply(*camera, true);
				outline_shader->SetUniformVec3("color", glm::vec3(0, 0.1f * (i + 1), 0));
				cube_mesh.Draw(*outline_shader);

				// glStencilMask(0x00)��������ֹģ�建���д�룬Ҳ����ֹ�����(glClear(stencil_buffer)��Ч)
				// �����Ҫ����Ϊ0xFF
				glStencilMask(0xFF);
				glEnable(GL_DEPTH_TEST);
				// �ر�ģ�����
				glDisable(GL_STENCIL_TEST);
			}
		}
#pragma endregion
*/
/*
#pragma region box ʵ������Ⱦ
		phong_instance_shader->Use();
		phong_instance_shader->Apply(*camera, true);
		phong_instance_shader->Apply(lights);
		phong_instance_shader->SetUniformBool("use_cube_map", false);
		phong_instance_shader->SetUniformBool("use_blinn", use_blinn);
		phong_instance_shader->Apply(cube_material);

		for (int i = 1; i < box_instance_num; i++)
		{
			auto model = box_instance_model[i];
			phong_instance_shader->SetUniformMat4f("model[" + std::to_string(i) + "]", model);
			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
			phong_instance_shader->SetUniformMat3f("NormalMatrix[" + std::to_string(i) + "]", normal_matrix);
		}
		cube_mesh.DrawInstance(*phong_instance_shader, box_instance_num);
#pragma endregion

#pragma region nanosuit

		Transform transform(glm::vec3(0, 0, 1), glm::vec3(0.15f));

		if (use_explode)
		{
			explode_shader->Use();
			explode_shader->Apply(*camera, true);
			explode_shader->Apply(lights);
			explode_shader->Apply(transform);
			explode_shader->Apply(*skybox_material.cube_map);
			explode_shader->SetUniformFloat("explode_length", std::sin(glfwGetTime()) + 1);
			nanosuit.Draw(*explode_shader);
		}
		else
		{
			phong_shader->Use();
			phong_shader->Apply(*camera, true);
			phong_shader->Apply(lights);
			phong_shader->Apply(transform);
			phong_shader->Apply(*skybox_material.cube_map);
			phong_shader->SetUniformBool("use_blinn", use_blinn);
			nanosuit.Draw(*phong_shader);

			if (draw_normal)
			{
				draw_normal_shader->Use();
				draw_normal_shader->Apply(*camera, true);
				draw_normal_shader->Apply(transform);
				draw_normal_shader->SetUniformFloat("normal_length", 0.1);
				draw_normal_shader->SetUniformVec3("color", 1.0f, 1.0f, 0);
				nanosuit.Draw(*draw_normal_shader);
			}
		}
#pragma endregion

#pragma region  light box
		for (int i = 0; i < lights.size(); i++)
		{
			Light* light = lights[i];

			light_shaderProgram->Use();
			light_shaderProgram->Apply(*camera, true);
			light_shaderProgram->SetUniformVec3("lightColor", light->diffuse);

			transform = Transform(light->position, glm::vec3(0.2f));
			light_shaderProgram->Apply(transform);
			light_shaderProgram->Apply(empty_material);

			light_mesh.Draw(*light_shaderProgram);
		}
#pragma endregion

#pragma region planet
		phong_shader->Use();
		phong_shader->Apply(*camera, true);
		phong_shader->Apply(lights);
		phong_shader->Apply(planet_tf);
		phong_shader->SetUniformBool("use_cube_map", false);
		phong_shader->SetUniformBool("use_blinn", use_blinn);
		planet.Draw(*phong_shader);

		if (rock_move)
		{
			for (int i = 0; i < rock_num; i++)
			{
				rock_tfs[i].position = planet_tf.position +
					glm::vec3(
						rock_radius[i] * std::sin(rock_angle[i] + glfwGetTime() * rock_around_speed),
						rock_y[i],
						rock_radius[i] * std::cos(rock_angle[i] + glfwGetTime() * rock_around_speed));
				if (planet_use_instance)
					rock_models[i] = rock_tfs[i].GetModel();
			}
		}
		if (planet_use_instance)
		{
			glBindBuffer(GL_ARRAY_BUFFER, rock_model_instance_vbo);
			glBufferData(GL_ARRAY_BUFFER, rock_num * sizeof(glm::mat4), rock_models.data(), GL_STATIC_DRAW);

			phong_instance_array_shader->Use();
			phong_instance_array_shader->Apply(*camera, true);
			phong_instance_array_shader->Apply(lights);
			phong_instance_array_shader->SetUniformBool("use_cube_map", false);
			phong_instance_array_shader->SetUniformBool("use_blinn", use_blinn);
			rock.DrawInstance(*phong_instance_array_shader, rock_num);
		}
		else
		{
			for (int i = 0; i < rock_num; i++)
			{
				phong_shader->Apply(rock_tfs[i]);
				rock.Draw(*phong_shader);
			}
		}
#pragma endregion

#pragma region skybox
		if (active_skybox)
		{
			// ��պе�����ڶ�����ɫ���б�ǿ����Ϊ1������Ȼ����Ĭ��ֵҲΪ1
			// ����������Ҫ��֤��պ���ֵС�ڻ������Ȼ��������С��ʱͨ����Ȳ���
			glDepthFunc(GL_LEQUAL);
			skybox_shader->Use();
			skybox_shader->Apply(*camera, false, true);
			skybox_shader->Apply(skybox_material);
			skybox_mesh.Draw(*skybox_shader);
			glDepthFunc(GL_LESS);
		}
#pragma endregion

#pragma region ��͸���ȵ�����
		glDisable(GL_CULL_FACE);
		std::vector<Object> objs(3);

		transform = Transform(glm::vec3(0, 0, 2), glm::vec3(1.0f));
		objs.emplace(objs.end(), &grass_mesh, transform, grass_material);

		for (int i = 0; i < 2; i++)
		{
			transform = Transform(glm::vec3(0, 0, 4- i), glm::vec3(1.0f));
			objs.emplace(objs.end(), &window_mesh, transform, window_material);
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
			blend_shader->Apply(*camera, true);
			blend_shader->Apply(it->second->transform);
			blend_shader->Apply(it->second->material);
			it->second->Draw(*blend_shader);
		}
#pragma endregion
*/
		// glfwSwapBuffers�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壩��������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ��
		glfwSwapBuffers(window);
		// glfwPollEvents���������û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����
		glfwPollEvents();
	}

	// ����Ⱦѭ��������������Ҫ��ȷ�ͷ�/ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}