//请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
//GLAD的头文件包含了正确的OpenGL头文件（例如GL / gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD

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
#include "GBuffer.h"

const int ScreenWidth = 1600;
const int ScreenHeight = 1200;

Camera* camera = nullptr;
FPSController* fps_crtl = nullptr;

bool firstMouse = true;
glm::vec2 mouseLastPos = 0.5f * glm::vec2(ScreenWidth, ScreenHeight);

void InitGLFW()
{
	glfwInit();
	// 使用glfwWindowHint函数来配置GLFW

	// 主版本号(Major)和次版本号(Minor)都设为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// 告诉GLFW使用核心模式(Core-profile)
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
/// 当用户改变窗口的大小的时候，视口也应该被调整
/// </summary>
/// <param name="window"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	// glfwGetKey函数，需要一个窗口以及一个按键作为输入。这个函数将会返回这个按键是否正在被按下
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// glfwSetwindowShouldClose把WindowShouldClose属性设置为true来关闭GLFW
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
		glm::vec3(0.0f,  0.0f,  2.0f),
		glm::vec3(8, 2.5, -2),
		glm::vec3(1, 2.5, -9),
		glm::vec3(1, 0.5, -2),
		glm::vec3(3, 1, -4)
	};

	std::vector<glm::vec3> lightColors = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	std::vector<Light*> lights = std::vector<Light*>();
	for (int i = 0; i < 5; i++)
	{
		Light* light = Light::CreatePoint(pointLightPositions[i], 1, 0.09f, 0.032f);
		lights.push_back(light);
	}
	lights[0]->ambient = glm::vec3(1);
	lights[0]->diffuse = glm::vec3(1);
	lights[0]->specular = glm::vec3(1);

	for (int i = 1; i < 5; i++)
	{
		lights[i]->ambient = 0.1f * lightColors[i - 1];
		lights[i]->diffuse = lightColors[i - 1];
		lights[i]->specular = lightColors[i - 1];
		lights[i]->linear = 0.1;
		lights[i]->quadratic = 0.03;
	}
	return lights;
}

int main() 
{
	srand(1024);

	InitGLFW();

	// 在创建窗口之前调用glfwWindowHint, 使用一个包含N个样本的多重采样缓冲
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	// glfwCreateWindow函数需要窗口的宽和高作为它的前两个参数。第三个参数表示这个窗口的名称（标题）
	GLFWwindow* window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Learn", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 通知GLFW将我们窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 告诉GLFW，它应该隐藏光标，并捕捉(Capture)它
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 注册函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// 注册回调函数，鼠标一移动MouseCallback函数就会被调用
	glfwSetCursorPosCallback(window, MouseMoveCallback);

	// 注册回调函数，鼠标滚轮
	glfwSetScrollCallback(window, MouseScrollCallback);

	// glad初始化要在配置opengl context之后执行
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
	ShaderProgram* phong_shadow_shader = new ShaderProgram("./Shader/MVP_Shadow.vert", "./Shader/Blinn_Phong_Shadow.frag");
	ShaderProgram* cube_map_depth_shader = new ShaderProgram("./Shader/ToWorldPosition.vert", "./Shader/ToSixLightSpace.geo", "./Shader/CalcDepth.frag");
	ShaderProgram* phong_cube_map_shadow_shader = new ShaderProgram("./Shader/MVP.vert", "./Shader/Blinn_Phong_CubeMap_Shadow.frag");
	ShaderProgram* skybox_depth_shader = new ShaderProgram("./Shader/skybox.vert", "./Shader/skybox_depth.frag");
	ShaderProgram* phong_TBN_shader = new ShaderProgram("./Shader/MVP_TBN.vert", "./Shader/Blinn_Phong_TBN.frag");
	ShaderProgram* phong_parallax_shader = new ShaderProgram("./Shader/MVP_TBN.vert", "./Shader/Blinn_Phong_ParallaxMapping.frag");
	ShaderProgram* phong_CubeMapShadow_Parallax_shader = new ShaderProgram("./Shader/MVP_TBN.vert", "./Shader/Binn_Phong_CubeMapShadow_ParalaxMapping.frag");
	ShaderProgram* HDR_tex_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/Reinhard_Tone_Mapping.frag");
	ShaderProgram* brightness_extract_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/Brightness_Extract.frag");
	ShaderProgram* gaussian_blur_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/Gaussian_Blur.frag");
	ShaderProgram* hdr_bloom_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/HDR_Bloom.frag");

	ShaderProgram* GBuffer_shader = new ShaderProgram("./Shader/MVP.vert", "./Shader/GBuffer.frag");
	ShaderProgram* phong_GBuffer_shader = new ShaderProgram("./Shader/simple.vert", "./Shader/Blinn_Phong_GBuffer.frag");

	Material empty_material;

	//camera = new OrthoCamera(-10, 10, -10, 10, 0.1, 100);
	camera = new PerspectiveCamera(45.0f, (float)ScreenWidth / ScreenHeight);
	fps_crtl = new FPSController(camera, window);
	// 给camera创建一个uniform缓冲对象
	camera->uniform_matrices.GenBuffer(2 * sizeof(glm::mat4), 0);

	std::vector<glm::vec3> square_position{
		glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0)
	};
	std::vector<glm::vec3> square_normal{
		glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)
	};
	std::vector<glm::vec2> square_texcoords{
		glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)
	};
	std::vector<Vertex> square_vertices
	{
		Vertex {glm::vec3(-1, -1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)},
		Vertex {glm::vec3(1, -1, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0)},
		Vertex {glm::vec3(1, 1, 0), glm::vec3(0, 0, 1), glm::vec2(1, 1)},
		Vertex {glm::vec3(-1, 1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
	};
	std::vector<unsigned int> square_indices{ 0, 1, 2, 2, 3, 0 };

	Mesh square_mesh(square_vertices, square_indices);
	Mesh square_mesh2(square_position, square_indices, &square_normal, &square_texcoords, true);

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

	#pragma region nanosuit
	Model nanosuit("./Model/nanosuit_reflection/nanosuit.obj");
	for (int i = 0; i < nanosuit.materials.size(); i++)
	{
		nanosuit.materials[i].shininess = 0.4f * 128;
		nanosuit.materials[i].environment_map = &skybox_texture;
	}
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
	Texture2D specular_map = Texture2D("Image/container2_specular.png", "texture_specular");
	Texture2D emission_map = Texture2D("Image/matrix.jpg", "texture_emission");

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
	glm::vec3 cube_rotate_axis[10];
	float cube_rotate_angle[10];
	for (int i = 0; i < 10; i++)
	{
		cube_rotate_axis[i] = glm::vec3(Utils::RandomFloat(-1, 1), Utils::RandomFloat(-1, 1), Utils::RandomFloat(-1, 1));
		cube_rotate_angle[i] = Utils::RandomInt(0, 360);
	}
#pragma endregion

#pragma region grass model
	std::vector<Texture2D> grass_texture{ Texture2D("./Image/grass.png", "texture_diffuse") };
	std::vector<Texture2D> window_Texture{ Texture2D("./Image/red_window.png", "texture_diffuse") };

	Material grass_material(grass_texture);
	Material window_material(window_Texture);
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

#pragma region brick wall
	Texture2D wall_diffuse("Image/brickwall.jpg", "texture_diffuse");
	Texture2D wall_specular("Image/brickwall_specular.jpg", "texture_specular");
	Texture2D wall_normal("Image/brickwall_normal.jpg", "texture_normal");
	Material wall_material(std::vector<Texture2D>{wall_diffuse, wall_specular }, 0.4f * 128, 0);
	Material wall_material_with_normal(std::vector<Texture2D>{wall_diffuse, wall_specular, wall_normal }, 0.4f * 128, 0);
#pragma endregion

#pragma region brick wall for parallax mapping
	Texture2D bricks2_diffuse("Image/bricks2.jpg", "texture_diffuse");
	Texture2D bricks2_specular("Image/brickwall_specular.jpg", "texture_specular");
	Texture2D bricks2_normal("Image/bricks2_normal.jpg", "texture_normal");
	Texture2D bricks2_parallax("Image/bricks2_disp.jpg", "texture_parallax");

	Material bricks2_material(std::vector<Texture2D>{bricks2_diffuse, bricks2_specular, bricks2_normal, bricks2_parallax}, 0.4f * 128, 0);

	std::vector<glm::vec3> bricks2_pos = {
		glm::vec3(7, 2.5, -2), glm::vec3(9, 2.5, -2),
		glm::vec3(8, 1.5, -2), glm::vec3(8, 3.5, -2),
		glm::vec3(7, 2.5, 0), glm::vec3(9, 2.5, 0),
		glm::vec3(8, 1.5, 0), glm::vec3(8, 3.5, 0),
		glm::vec3(7, 2.5, 2), glm::vec3(9, 2.5, 2),
		glm::vec3(8, 1.5, 2), glm::vec3(8, 3.5, 2),
		glm::vec3(7, 2.5, 4), glm::vec3(9, 2.5, 4),
		glm::vec3(8, 1.5, 4), glm::vec3(8, 3.5, 4),
		glm::vec3(7, 2.5, 6), glm::vec3(9, 2.5, 6),
		glm::vec3(8, 1.5, 6), glm::vec3(8, 3.5, 6)
	};
	std::vector<glm::vec3> bricks2_rotate_axis = {
		glm::vec3(0, 1, 0), glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0), glm::vec3(1, 0, 0)
	};
	std::vector<float> bricks2_rotate_angle = {
		90, -90, -90, 90
	};

	int bricks2_count = bricks2_pos.size();
#pragma endregion

	Time::Init();
	
	auto lights = CreateLight();

	// 启用面剔除
	glEnable(GL_CULL_FACE);
	// glCullFace函数有三个可用的选项：
	//GL_BACK：只剔除背向面。
	//	GL_FRONT：只剔除正向面。
	//	GL_FRONT_AND_BACK：剔除正向面和背向面
	glCullFace(GL_BACK);
	// glFrontFace定义正面的环绕顺序
	// GL_CCW：逆时针，GL_CW：顺时针
	// 默认：GL_CCW
	glFrontFace(GL_CCW);

	//// 启用混合
	//glEnable(GL_BLEND);
	//// glBlendFunc函数接受两个参数，来设置源和目标因子
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 启用多重采样。在大多数OpenGL的驱动上，多重采样都是默认启用
	glEnable(GL_MULTISAMPLE);

#pragma region 深度贴图帧缓冲
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	TexParams depth_tex_params = {
			{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
			{GL_TEXTURE_MAG_FILTER, GL_NEAREST},
			{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
			{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},
			{GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE}
	};

	// 创建与光源相同数量的阴影
	std::vector<Shadow*> shadows;
	for (int i = 0; i < lights.size(); i++)
	{
		Shadow* shadow = new PointShadow(SHADOW_WIDTH, SHADOW_HEIGHT, lights[i], depth_tex_params, 0.1f, 25.0f);
		shadows.push_back(shadow);
	}
#pragma endregion

	GBuffer g_buffer(ScreenWidth, ScreenHeight);

	bool use_box_outline = false;
	bool active_skybox = false;
	bool use_explode = false;
	bool draw_normal = false;
	bool rock_move = true;
	bool use_blinn = true;

	bool light_follow_camera = false;
	bool use_frame_buffer = true;

	std::cout << "开始渲染" << std::endl;
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);
		fps_crtl->Update();
		light_follow_camera = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
		if (light_follow_camera)
		{
			lights[0]->position = camera->position;
		}
	
		camera->FillUniformMatrices();
		
		g_buffer.BindSelf();
		g_buffer.UpdateViewport();
		GLuint attachments1[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(4, attachments1);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		GBuffer_shader->Use();
		GBuffer_shader->Apply(*camera, true, false);

		Transform tf(glm::vec3(0, 0, 1), glm::vec3(0.1f));
		GBuffer_shader->Apply(tf);
		nanosuit.Draw(*GBuffer_shader);

		for (int i = 0; i < 10; i++)
		{
			Transform box_tf(cubePositions[i], glm::vec3(1), cube_rotate_axis[i], cube_rotate_angle[i]);
			GBuffer_shader->Apply(box_tf);
			GBuffer_shader->Apply(cube_material);
			cube_mesh.Draw(*GBuffer_shader);
		}
		
		GLuint attachments2[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, attachments2);

		GLObject::Unbind<FrameBuffer>();
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// 
		phong_GBuffer_shader->Use();
		phong_GBuffer_shader->Apply(lights);
		phong_GBuffer_shader->Apply(g_buffer);
		phong_GBuffer_shader->Apply(*camera);
		square_mesh.Draw(*phong_GBuffer_shader);

		//frame_buffer_shader->Use();
		//frame_buffer_shader->Apply(g_buffer.gDiffuse, "tex");
		//square_mesh.Draw(*frame_buffer_shader);
		Texture::ClearAllBindTexture();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer.GetID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		tf.position = glm::vec3(-2, 0, 1);
		phong_shader->Use();
		phong_shader->Apply(*camera, true);
		phong_shader->Apply(lights);
		phong_shader->Apply(tf);
		phong_shader->SetUniformBool("use_blinn", use_blinn);
		nanosuit.Draw(*phong_shader);

		#pragma region  light box
		for (int i = 0; i < lights.size(); i++)
		{
			Light* light = lights[i];

			light_shaderProgram->Use();
			light_shaderProgram->Apply(*camera, true);
			light_shaderProgram->SetUniformVec3("lightColor", light->diffuse);

			Transform transform = Transform(light->position, glm::vec3(0.1f));
			light_shaderProgram->Apply(transform);
			light_shaderProgram->Apply(empty_material);

			light_mesh.Draw(*light_shaderProgram);
			Texture::ClearAllBindTexture();

		}
		#pragma endregion

		/*
#pragma region 深度贴图
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		for (int k = 0; k < lights.size(); k++) 
		{
			shadows[k]->Clear();
			cube_map_depth_shader->Use();
			cube_map_depth_shader->Apply(*shadows[k]);

			for (int i = 0; i < 10; i++)
			{
				float angle = 29 * i;
				Transform tf(cubePositions[i], glm::vec3(1), glm::vec3(1.0f, 0.3f, 0.5f), angle);
				cube_map_depth_shader->Apply(tf);
				cube_mesh.Draw(*cube_map_depth_shader);
			}
			for (int i = 0; i < bricks2_count; i++)
			{
				Transform bricks2_tf(bricks2_pos[i], glm::vec3(1), bricks2_rotate_axis[i % 4], bricks2_rotate_angle[i % 4]);
				cube_map_depth_shader->Apply(bricks2_tf);
				square_mesh2.Draw(*cube_map_depth_shader);
			}
		}
		GLObject::Unbind<FrameBuffer>();
		glViewport(0, 0, ScreenWidth, ScreenHeight);
#pragma endregion
		
		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0, 0, 0, 1.0f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		// 每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
		
		glCullFace(GL_BACK);

#pragma region box
		for (int i = 0; i < 10; i++)
		{
			// glEnable和glDisable函数允许我们启用或禁用某个OpenGL功能。这个功能会一直保持启用/禁用状态，直到另一个调用来禁用/启用它
			// 启用深度测试，需要开启GL_DEPTH_TEST
			glEnable(GL_DEPTH_TEST);

			float angle = 29 * i;
			Transform tf(cubePositions[i], glm::vec3(1), glm::vec3(1.0f, 0.3f, 0.5f), angle);
			// 更新一个uniform之前你必须先使用shader程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的
			phong_cube_map_shadow_shader->Use();
			phong_cube_map_shadow_shader->Apply(tf);
			phong_cube_map_shadow_shader->Apply(*camera, true);
			phong_cube_map_shadow_shader->Apply(lights);
			phong_cube_map_shadow_shader->SetUniformBool("use_cube_map", false);
			phong_cube_map_shadow_shader->SetUniformBool("use_blinn", use_blinn);
			phong_cube_map_shadow_shader->Apply(cube_material);

			for (int j = 0; j < lights.size(); j++) 
			{
				phong_cube_map_shadow_shader->Apply(*shadows[j], j);
			}
			////glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。第二个参数指定了顶点数组的起始索引。最后一个参数指定我们打算绘制多少个顶点
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*phong_cube_map_shadow_shader);
		}
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
			phong_TBN_shader->Use();
			phong_TBN_shader->Apply(*camera, true);
			phong_TBN_shader->Apply(lights);
			phong_TBN_shader->Apply(transform);
			phong_TBN_shader->Apply(*skybox_material.cube_map);
			phong_TBN_shader->SetUniformBool("use_blinn", use_blinn);
			nanosuit.Draw(*phong_TBN_shader);

			transform.position = glm::vec3(-2, 0, 1);
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

*/
		// glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
		// glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
		glfwPollEvents();
	}

	// 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}