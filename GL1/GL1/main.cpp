//请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
//GLAD的头文件包含了正确的OpenGL头文件（例如GL / gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD

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

#pragma region 配置光源
	auto lights = CreateLight();
#pragma endregion
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


#pragma region 创建帧缓冲
	const float frame_buffer_scale = 0.07f;
	const int frame_buffer_width = ScreenWidth * frame_buffer_scale;
	const int frame_buffer_height = ScreenHeight * frame_buffer_scale;

	unsigned int frame_buffer;
	// 创建一个帧缓冲对象
	glGenFramebuffers(1, &frame_buffer);
	// glBindFramebuffer: 绑定帧缓冲, 绑定到GL_FRAMEBUFFER目标之后，所有的读取和写入帧缓冲的操作将会影响当前绑定的帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	// 给帧缓冲创建附件的时候，我们有两个选项：纹理或渲染缓冲对象

	// 附加纹理
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	// 我们给纹理的data参数传递了NULL, 对于这个纹理，我们仅仅分配了内存而没有填充它。填充这个纹理将会在我们渲染到帧缓冲之后来进行
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_buffer_width, frame_buffer_height, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// 核在对屏幕纹理的边缘进行采样的时候，由于还会对中心像素周围的8个像素进行采样，其实会取到纹理之外的像素。
	// 由于环绕方式默认是GL_REPEAT，所以在没有设置的情况下取到的是屏幕另一边的像素，而另一边的像素本不应该对中心像素产生影响，这就可能会在屏幕边缘产生很奇怪的条纹。
	// 为了消除这一问题，我们可以将屏幕纹理的环绕方式都设置为GL_CLAMP_TO_EDGE。
	// 这样子在取到纹理外的像素时，就能够重复边缘的像素来更精确地估计最终的值了。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	// 将纹理附加到帧缓冲上， 之后所有的渲染指令将会写入到这个纹理中
	// glFramebufferTexture2D参数：
	// target：帧缓冲的目标（绘制、读取或者两者皆有）
	// attachment：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的0意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
	//	textarget：你希望附加的纹理类型
	// texture：要附加的纹理id
	//	level：mipmap的级别
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// 附加渲染缓冲对象
	// 渲染缓冲对象的数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过
	// 当写入或者复制它的数据到其它缓冲中时是非常快的。所以，交换缓冲这样的操作在使用渲染缓冲对象时会非常快
	// 渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问）
	// 由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，只关心深度和模板测试
	unsigned int rbo;
	// 创建一个渲染缓冲对象
	glGenRenderbuffers(1, &rbo);
	// 绑定渲染缓冲对象rbo到GL_RENDERBUFFER，让之后所有的渲染缓冲操作影响当前的rbo
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// 给渲染缓冲对象分配内存。
	// GL_DEPTH24_STENCIL8作为内部格式，它封装了24位的深度和8位的模板缓冲，将该渲染缓冲对象用于深度和模板渲染缓冲
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frame_buffer_width, frame_buffer_height);
	// 解绑
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// 将渲染缓冲对象附加到帧缓冲上
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// 检查帧缓冲是否是完整的，如果不是，将打印错误信息。
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// 解绑帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Mesh frame_buffer_mesh(square_vertices, square_indices, std::vector<Texture2D>());
#pragma endregion

	bool use_frame_buffer = false;
	std::cout << "开始渲染" << std::endl;
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);
		camera->Update();

		if (use_frame_buffer)
		{
			glViewport(0, 0, frame_buffer_width, frame_buffer_height);
			// 绑定帧缓冲, 让之后的渲染影响当前绑定的帧缓冲
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		}

		glEnable(GL_DEPTH_TEST);

		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		// 每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
		//spot_light->position = camera->position;
		//spot_light->direction = camera->Front;

#pragma region box
		for (int i = 0; i < 10; i++)
		{
			// glEnable和glDisable函数允许我们启用或禁用某个OpenGL功能。这个功能会一直保持启用/禁用状态，直到另一个调用来禁用/启用它
			// 启用深度测试，需要开启GL_DEPTH_TEST
			glEnable(GL_DEPTH_TEST);
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

			glm::mat4 model = glm::mat4(1.0f); // 通过将顶点坐标乘以模型矩阵，我们将该顶点坐标变换到世界坐标
			model = glm::translate(model, cubePositions[i]);

			float angle = 29 * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			// 更新一个uniform之前你必须先使用shader程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的
			phong_shader->Use();
			phong_shader->SetUniformMat4f("model", model);
			phong_shader->Apply(*camera);
			phong_shader->SetUniformFloat("material.shininess", 0.4f * 128);
			phong_shader->Apply(lights);

			// 计算法线矩阵，用于把法向量转换为世界空间坐标
			// 法线应该只受缩放和旋转变换的影响，而不受位移
			// 不等比缩放会导致法向量不再垂直于对应的表面
			// 因此不能直接用模型矩阵对法向量做变换，而是使用一个为法向量专门定制的模型矩阵。这个矩阵称之为法线矩阵：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
			phong_shader->SetUniformMat3f("NormalMatrix", normal_matrix);

			////glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。第二个参数指定了顶点数组的起始索引。最后一个参数指定我们打算绘制多少个顶点
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*phong_shader);

			// 使用GL_ALWAYS模板测试函数，我们保证了片段永远会通过模板测试，在绘制片段的地方，模板缓冲会被更新为参考值1
			// 模板函数设置为GL_NOTEQUAL：当前模板缓冲值不为1的片元才能通过模板测试
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			// 将掩码值设为0， 禁止模板缓冲更新
			glStencilMask(0x00);
			//glDisable(GL_DEPTH_TEST);

			model = glm::scale(model, glm::vec3(1.05));
			outline_shader->Use();
			outline_shader->SetUniformMat4f("model", model);
			outline_shader->Apply(*camera);
			outline_shader->SetUniformVec3("color", glm::vec3(0, 0.1f * (i + 1), 0));
			cube_mesh.Draw(*outline_shader);

			// glStencilMask(0x00)不仅会阻止模板缓冲的写入，也会阻止其清空(glClear(stencil_buffer)无效)
			// 因此需要重设为0xFF
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			// 关闭模板测试
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
		// 天空盒的深度在顶点着色器中被强制设为1，但深度缓冲的默认值也为1
		// 所以我们需要保证天空盒在值小于或等于深度缓冲而不是小于时通过深度测试
		glDepthFunc(GL_LEQUAL);
		skybox_shader->Use();
		skybox_shader->SetUniformInt("cube_map", 0);
		skybox_shader->Apply(*camera, true);
		glActiveTexture(GL_TEXTURE0);
		skybox_texture.Bind();
		skybox_mesh.Draw(*skybox_shader);
		glDepthFunc(GL_LESS);
#pragma endregion

#pragma region 带透明度的物体
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
			// 此时场景的渲染结果已经输出到帧缓冲的附加纹理上了
			// 解绑帧缓冲
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			// 清除屏幕的颜色和缓冲
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// 将附加纹理作为贴图，渲染一个四边形
			frame_buffer_shader->Use();
			frame_buffer_shader->SetUniformInt("tex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			frame_buffer_mesh.Draw(*frame_buffer_shader);
		}

		// glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
		// glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
		glfwPollEvents();
	}

	// 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}