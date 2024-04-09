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


	camera = new Camera(window, 45.0f, (float)ScreenWidth / ScreenHeight);

	Model nanosuit("./Model/nanosuit/nanosuit.obj");

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
	std::vector<Vertex> square_vertices
	{
		Vertex {glm::vec3(-0.5, -0.5, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
		Vertex {glm::vec3(0.5, -0.5, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
		Vertex {glm::vec3(0.5, 0.5, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1)},
		Vertex {glm::vec3(-0.5, 0.5, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1)},
	};
	std::vector<unsigned int> square_indices{ 0, 1, 2, 2, 3, 0 };
	
	std::vector<Texture2D> grass_texture{ Texture2D("./Image/grass.png", "texture_diffuse") };
	std::vector<Texture2D> window_Texture{ Texture2D("./Image/red_window.png", "texture_diffuse") };

	Mesh grass_mesh(square_vertices, square_indices, grass_texture);
	Mesh window_mesh(square_vertices, square_indices, window_Texture);
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

	std::cout << "开始渲染" << std::endl;
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);

		glEnable(GL_DEPTH_TEST);

		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		// 每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色

		camera->Update();

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

#pragma region 带透明度的物体
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

		// glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
		// glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
		glfwPollEvents();
	}

	// 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}