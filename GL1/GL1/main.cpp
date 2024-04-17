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

	Material empty_material;

	camera = new OrthoCamera(-10, 10, -10, 10, 0.1, 100);
	//camera = new PerspectiveCamera(45.0f, (float)ScreenWidth / ScreenHeight);
	fps_crtl = new FPSController(camera, window);
	// 给camera创建一个uniform缓冲对象
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

	// 启用混合
	glEnable(GL_BLEND);
	// glBlendFunc函数接受两个参数，来设置源和目标因子
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 启用多重采样。在大多数OpenGL的驱动上，多重采样都是默认启用
	glEnable(GL_MULTISAMPLE);

	Mesh frame_buffer_mesh(square_vertices, square_indices);

#pragma region 深度贴图帧缓冲
	FrameBuffer depth_map_buffer(1024, 1024);

	TexParams depth_tex_params = {
		{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
		{GL_TEXTURE_MAG_FILTER, GL_NEAREST},
		{GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}
	};
	// 因为只关心深度值，把纹理格式指定为GL_DEPTH_COMPONENT
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

	std::cout << "开始渲染" << std::endl;
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);
		fps_crtl->Update();
		camera->FillUniformMatrices();

		glEnable(GL_DEPTH_TEST);

		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		// 每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
		
#pragma region 深度贴图
		depth_map_buffer.UpdateViewport();
		depth_map_buffer.Bind();
		// 绑定帧缓冲后要清除之前的缓存
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

		// 解绑帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 此时场景的渲染结果已经输出到帧缓冲的附加纹理上了
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glDisable(GL_DEPTH_TEST);
		// 将附加纹理作为贴图，渲染一个四边形
		depth_texture_shader->Use();
		depth_texture_shader->SetUniformInt("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_map_buffer.color_buffer->GetID());
		frame_buffer_mesh.Draw(*depth_texture_shader);


/*
#pragma region box
		for (int i = 0; i < 10; i++)
		{
			// glEnable和glDisable函数允许我们启用或禁用某个OpenGL功能。这个功能会一直保持启用/禁用状态，直到另一个调用来禁用/启用它
			// 启用深度测试，需要开启GL_DEPTH_TEST
			glEnable(GL_DEPTH_TEST);

			if (use_box_outline)
			{
				// 启用模板测试
				glEnable(GL_STENCIL_TEST);
				// 设置测试通过或失败时的行为
				// 第一个参数：模板测试失败时采取的行为, 
				// 第二个参数：模板测试通过，但深度测试失败时采取的行为, 
				// 第三个参数：模板测试和深度测试都通过时采取的行为
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				// 设置模板函数
				// 第一个参数：设置模板测试函数(Stencil Test Function)。这个测试函数将会应用到已储存的模板值上和glStencilFunc函数的ref值上。
				//		可用的选项有：GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL和GL_ALWAYS
				// 第二个参数：设置了模板测试的参考值。模板缓冲的内容将会与这个值进行比较
				// 第三个参数：设置一个掩码，它将会与参考值和储存的模板值在测试比较它们之前进行与(AND)运算。初始情况下所有位都为1
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				// 设置模板缓冲更新前的掩码，它会与将要写入缓冲的模板值进行与(AND)运算
				glStencilMask(0xFF);
			}
			
			float angle = 29 * i;
			Transform tf(cubePositions[i], glm::vec3(1), glm::vec3(1.0f, 0.3f, 0.5f), angle);
			// 更新一个uniform之前你必须先使用shader程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的
			phong_shader->Use();
			phong_shader->Apply(tf);
			phong_shader->Apply(*camera, true);
			phong_shader->Apply(lights);
			phong_shader->SetUniformBool("use_cube_map", false);
			phong_shader->SetUniformBool("use_blinn", use_blinn);
			phong_shader->Apply(cube_material);
			////glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。第二个参数指定了顶点数组的起始索引。最后一个参数指定我们打算绘制多少个顶点
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*phong_shader);

			if (use_box_outline)
			{
				// 使用GL_ALWAYS模板测试函数，我们保证了片段永远会通过模板测试，在绘制片段的地方，模板缓冲会被更新为参考值1
				// 模板函数设置为GL_NOTEQUAL：当前模板缓冲值不为1的片元才能通过模板测试
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				// 将掩码值设为0， 禁止模板缓冲更新
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);

				tf.scale = glm::vec3(1.05);
				outline_shader->Use();
				outline_shader->Apply(tf);
				outline_shader->Apply(*camera, true);
				outline_shader->SetUniformVec3("color", glm::vec3(0, 0.1f * (i + 1), 0));
				cube_mesh.Draw(*outline_shader);

				// glStencilMask(0x00)不仅会阻止模板缓冲的写入，也会阻止其清空(glClear(stencil_buffer)无效)
				// 因此需要重设为0xFF
				glStencilMask(0xFF);
				glEnable(GL_DEPTH_TEST);
				// 关闭模板测试
				glDisable(GL_STENCIL_TEST);
			}
		}
#pragma endregion
*/
/*
#pragma region box 实例化渲染
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
			// 天空盒的深度在顶点着色器中被强制设为1，但深度缓冲的默认值也为1
			// 所以我们需要保证天空盒在值小于或等于深度缓冲而不是小于时通过深度测试
			glDepthFunc(GL_LEQUAL);
			skybox_shader->Use();
			skybox_shader->Apply(*camera, false, true);
			skybox_shader->Apply(skybox_material);
			skybox_mesh.Draw(*skybox_shader);
			glDepthFunc(GL_LESS);
		}
#pragma endregion

#pragma region 带透明度的物体
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
		// glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
		// glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
		glfwPollEvents();
	}

	// 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}