//请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
//GLAD的头文件包含了正确的OpenGL头文件（例如GL / gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD

#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "ShaderProgram.h"
#include "stb_image.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Time.h"
#include "Light.h"
#include "Mesh.h"

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
	ShaderProgram* shaderProgram = new ShaderProgram("./Shader/shader.vert", "./Shader/shader.frag");

	camera = new Camera(window, 45.0f, (float)ScreenWidth / ScreenHeight);

	float vertices[] = {
		// positions          // normals           // texture coords
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

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
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	std::vector<Vertex> _vertices;
	for (int i = 0; i < 36; i++)
	{
		Vertex v{
			glm::vec3(vertices[i * 8], vertices[i * 8 + 1], vertices[i * 8 + 2]),
			glm::vec3(vertices[i * 8 +3], vertices[i * 8 + 4], vertices[i * 8 + 5]),
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

	// glEnable和glDisable函数允许我们启用或禁用某个OpenGL功能。这个功能会一直保持启用/禁用状态，直到另一个调用来禁用/启用它
	// 启用深度测试，需要开启GL_DEPTH_TEST
	glEnable(GL_DEPTH_TEST);

	Time::Init();

	glm::vec3 cubePosition = glm::vec3(0, 0, -3);

#pragma region 配置光源
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
	lights.push_back(spot_light);

	for (Light* light : lights)
	{
		light->ambient = glm::vec3(0.2f);
		light->diffuse = glm::vec3(1);
		light->specular = glm::vec3(1);
	}

	directional_light->diffuse = glm::vec3(1.0, 1.0, 0);
#pragma endregion

	std::cout << "开始渲染" << std::endl;
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);

		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		// 每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色

		camera->Update();

		//light.Update();
		spot_light->position = camera->position;
		spot_light->direction = camera->Front;

#pragma region MVP
		// mvp
		glm::mat4 view = camera->GetView();
		glm::mat4 projection = camera->GetProjection();
#pragma endregion

#pragma region Shader
		//Utils::PrintVec3(camera->position);
		shaderProgram->Use();

		//// 更新一个uniform之前你必须先使用shader程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的
		shaderProgram->SetUniformVec3("viewPos", camera->position);

		shaderProgram->SetUniformMat4f("view", view);
		shaderProgram->SetUniformMat4f("projection", projection);
		shaderProgram->SetUniformFloat("material.shininess", 0.4f * 128);

		for (int i = 0; i < lights.size(); i++)
		{
			Light* light = lights[i];

			std::string index = "lights[" + std::to_string(i) + "]";
			shaderProgram->SetUniformInt(index + ".type", (int)light->type);
			shaderProgram->SetUniformVec3(index + ".position", light->position);
			shaderProgram->SetUniformVec3(index + ".direction", light->direction);
			shaderProgram->SetUniformFloat(index + ".innerCutOff", glm::cos(glm::radians(light->innerCutOff)));
			shaderProgram->SetUniformFloat(index + ".outerCutOff", glm::cos(glm::radians(light->outerCutOff)));

			shaderProgram->SetUniformVec3(index + ".ambient", light->ambient);
			shaderProgram->SetUniformVec3(index + ".diffuse", light->diffuse);
			shaderProgram->SetUniformVec3(index + ".specular", light->specular);

			shaderProgram->SetUniformFloat(index + ".constant", light->type == LightType::Point ? light->constant : 1);
			shaderProgram->SetUniformFloat(index + ".linear", light->type == LightType::Point ? light->linear : 0);
			shaderProgram->SetUniformFloat(index + ".quadratic", light->type == LightType::Point ? light->quadratic : 0);
		}
#pragma endregion

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f); // 通过将顶点坐标乘以模型矩阵，我们将该顶点坐标变换到世界坐标
			model = glm::translate(model, cubePositions[i]);

			float angle = 29 * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shaderProgram->SetUniformMat4f("model", model);

			// 计算法线矩阵，用于把法向量转换为世界空间坐标
			// 法线应该只受缩放和旋转变换的影响，而不受位移
			// 不等比缩放会导致法向量不再垂直于对应的表面
			// 因此不能直接用模型矩阵对法向量做变换，而是使用一个为法向量专门定制的模型矩阵。这个矩阵称之为法线矩阵：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
			shaderProgram->SetUniformMat3f("NormalMatrix", normal_matrix);

			////glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。第二个参数指定了顶点数组的起始索引。最后一个参数指定我们打算绘制多少个顶点
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			cube_mesh.Draw(*shaderProgram);
		}

		for (int i = 0; i < lights.size() - 1; i++)
		{
			Light* light = lights[i];

			light_shaderProgram->Use();
			light_shaderProgram->SetUniformVec3("lightColor", light->diffuse);

			auto lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, light->position);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			light_shaderProgram->SetUniformMat4f("model", lightModel);
			light_shaderProgram->SetUniformMat4f("view", view);
			light_shaderProgram->SetUniformMat4f("projection", projection);

			light_mesh.Draw(*light_shaderProgram);
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