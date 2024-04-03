//��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���
//GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL / gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD

#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "stb_image.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Model.h"
#include "Camera.h"
#include "Time.h"

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

#pragma region MyRegion
	// EBO��һ��������������һ�����㻺��������һ�������洢 OpenGL ��������Ҫ������Щ���������
	unsigned int VBO[2], VAO[2];

	// glGenBuffers�����������ɻ��������������, ��һ��������Ҫ���ɵĻ��������������ڶ��������������洢����������Ƶ�����
	glGenBuffers(2, VBO);
	glGenVertexArrays(2, VAO);

	// glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindVertexArray(VAO[0]);
	
	/*
		glVertexAttribPointer��������OpenGL����ν�����������
		��һ������ָ������Ҫ���õĶ�������, ��Ӧ������ɫ���е� layout (location == ?)
		�ڶ�������ָ���������ԵĴ�С
		����������ָ�����ݵ�����
		���ĸ��������������Ƿ�ϣ�����ݱ���׼�����������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮��
		�����������������(Stride)�������������������Ķ���������֮��ļ������˵���Ǵ�������Եڶ��γ��ֵ�λ�õ���һ�γ��ֵ�λ��֮���ж����ֽ�
		���һ��������ʾλ�������ڻ�������ʼλ�õ�ƫ������������void*��������Ҫ����ǿ������ת��

		ÿ���������Դ�һ��VBO������ڴ��л���������ݣ��������Ǵ��ĸ�VBO�������п����ж��VBO����ȡ����ͨ���ڵ���glVertexAttribPointerʱ�󶨵�GL_ARRAY_BUFFER��VBO������
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray�Զ�������λ��ֵ��Ϊ���������ö������ԣ���������Ĭ���ǽ��õ�
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

#pragma endregion

	Texture2D diffuse_map = Texture2D("Image/container2.png");
	diffuse_map.SetParameters();
	diffuse_map.GenerateMipmap();

	Texture2D specular_map = Texture2D("Image/container2_specular.png");
	specular_map.SetParameters();
	specular_map.GenerateMipmap();

	// glEnable��glDisable���������������û����ĳ��OpenGL���ܡ�������ܻ�һֱ��������/����״̬��ֱ����һ������������/������
	// ������Ȳ��ԣ���Ҫ����GL_DEPTH_TEST
	glEnable(GL_DEPTH_TEST);

	Time::Init();

	glm::vec3 cubePosition = glm::vec3(0, 0, -3);
	glm::vec3 cubeColor = glm::vec3(1, 1, 1);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition;

	float round = 2;
	
	std::cout << "��ʼ��Ⱦ" << std::endl;
	// ���һ��whileѭ�������ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	// glfwWindowShouldClose����������ÿ��ѭ���Ŀ�ʼǰ���һ��GLFW�Ƿ�Ҫ���˳�������ǵĻ����ú�������true����Ⱦѭ����ֹͣ���У�֮�����ǾͿ��Թر�Ӧ�ó���
	while (!glfwWindowShouldClose(window))
	{
		Time::Update();

		ProcessInput(window);

		// glClearColor���������Ļ���õ���ɫ��������glClear�����������ɫ����֮��������ɫ���嶼�ᱻ���ΪglClearColor�������õ���ɫ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear���������Ļ����ɫ���壬������һ������λ(Buffer Bit)��ָ��Ҫ��յĻ��壬���ܵĻ���λ��GL_COLOR_BUFFER_BIT��GL_DEPTH_BUFFER_BIT��GL_STENCIL_BUFFER_BIT
		// ÿ����Ⱦ����֮ǰ�����Ȼ��壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glClearColor������һ��״̬���ú�������glClear��������һ��״̬ʹ�õĺ�������ʹ���˵�ǰ��״̬����ȡӦ�����Ϊ����ɫ

		camera->Update();

		lightPosition = cubePosition + glm::vec3(std::cos(glfwGetTime() * 2) * round, 1.5f, std::sin(glfwGetTime() * 2) * round);

#pragma region MVP
		// mvp
		glm::mat4 view = camera->GetView();
		glm::mat4 projection = camera->GetProjection();
#pragma endregion

#pragma region ����
		glActiveTexture(GL_TEXTURE0);
		diffuse_map.Bind();

		glActiveTexture(GL_TEXTURE1);
		specular_map.Bind();
#pragma endregion

#pragma region Shader
		//Utils::PrintVec3(camera->position);
		shaderProgram->Use();

		shaderProgram->SetUniformVec3("ourColor", cubeColor);
		shaderProgram->SetUniformVec3("lightColor", lightColor);
		shaderProgram->SetUniformVec3("lightPosition", lightPosition);
		shaderProgram->SetUniformVec3("viewPos", camera->position);

		shaderProgram->SetUniformMat4f("view", view);
		shaderProgram->SetUniformMat4f("projection", projection);

		glm::mat4 model = glm::mat4(1.0f); // ͨ���������������ģ�;������ǽ��ö�������任����������
		model = glm::translate(model, cubePosition);
		shaderProgram->SetUniformMat4f("model", model);

		// ���㷨�߾������ڰѷ�����ת��Ϊ����ռ�����
		// ����Ӧ��ֻ�����ź���ת�任��Ӱ�죬������λ��
		// ���ȱ����Żᵼ�·��������ٴ�ֱ�ڶ�Ӧ�ı���
		// ��˲���ֱ����ģ�;���Է��������任������ʹ��һ��Ϊ������ר�Ŷ��Ƶ�ģ�;�����������֮Ϊ���߾���ģ�;������Ͻ�3x3���ֵ�������ת�þ���
		glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
		shaderProgram->SetUniformMat3f("NormalMatrix", normal_matrix);
		//// ����һ��uniform֮ǰ�������ʹ��shader���򣨵���glUseProgram)����Ϊ�����ڵ�ǰ�������ɫ������������uniform��
		
		//shaderProgram->SetUniformVec3("material.ambient", 0.24725f, 0.1995f, 0.0745f);
		//shaderProgram->SetUniformVec3("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
		shaderProgram->SetUniformVec3("material.specular", 0.628281f, 0.555802f, 0.366065f);
		shaderProgram->SetUniformFloat("material.shininess", 0.4f * 128);
		shaderProgram->SetUniformInt("material.diffuse", 0);
		shaderProgram->SetUniformInt("material.specular", 1);

		shaderProgram->SetUniformVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		shaderProgram->SetUniformVec3("light.diffuse", lightColor);
		shaderProgram->SetUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
		shaderProgram->SetUniformVec3("light.position", lightPosition);
#pragma endregion

		glBindVertexArray(VAO[0]);

		////glDrawArrays������һ�����������Ǵ�����Ƶ�OpenGLͼԪ�����͡��ڶ�������ָ���˶����������ʼ���������һ������ָ�����Ǵ�����ƶ��ٸ�����
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		light_shaderProgram->Use();
		light_shaderProgram->SetUniformVec3("lightColor", lightColor);

		auto lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPosition);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		light_shaderProgram->SetUniformMat4f("model", lightModel);
		light_shaderProgram->SetUniformMat4f("view", view);
		light_shaderProgram->SetUniformMat4f("projection", projection);

		glBindVertexArray(VAO[1]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfwSwapBuffers�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壩��������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ��
		glfwSwapBuffers(window);
		// glfwPollEvents���������û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����
		glfwPollEvents();
	}

	// ����Ⱦѭ��������������Ҫ��ȷ�ͷ�/ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}