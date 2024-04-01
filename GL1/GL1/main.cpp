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

const int ScreenWidth = 800;
const int ScreenHeight = 600;

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

	// ע�ắ��������GLFW����ϣ��ÿ�����ڵ�����С��ʱ������������
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// glad��ʼ��Ҫ������opengl context֮��ִ��
	if (!InitGLAD()) return -1;

	ShaderProgram* shaderProgram = new ShaderProgram("./Shader/shader.vert", "./Shader/shader.frag");

	Camera camera = Camera();

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// ��������vertices���±�, ע��������0��ʼ
	unsigned int indices_1[36];
	for (int i = 0; i < 36; i++)
		indices_1[i] = i;

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

	Model* model1 = new Model(36, vertices, 36* 5, indices_1, 36);

	unsigned int VBO[2], VAO[2];

	// glGenBuffers�����������ɻ��������������, ��һ��������Ҫ���ɵĻ��������������ڶ��������������洢����������Ƶ�����
	glGenBuffers(2, VBO);
	glGenVertexArrays(2, VAO);

	// EBO��һ��������������һ�����㻺��������һ�������洢 OpenGL ��������Ҫ������Щ���������
	unsigned int EBO[2];
	// ����Ԫ�ػ������
	glGenBuffers(2, EBO);

	glBindVertexArray(VAO[0]);
	// glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	model1->BufferVertices();
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray�Զ�������λ��ֵ��Ϊ���������ö������ԣ���������Ĭ���ǽ��õ�
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	// ��EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	model1->BufferIndices();

	Texture2D texture1 = Texture2D("Image/container.jpg");
	texture1.SetParameters();
	texture1.GenerateMipmap();

	Texture2D texture2 = Texture2D("Image/awesomeface.png");
	texture2.SetParameters();
	texture2.GenerateMipmap();

	// glBindVertexArray����0��ʾ���ǰ��VAO
	// ����һ��VAOʱ��֮ǰ�󶨵�VAO���Զ��������ͨ������Ҫ�ֶ����һ��VAO
	glBindVertexArray(0);

	// glEnable��glDisable���������������û����ĳ��OpenGL���ܡ�������ܻ�һֱ��������/����״̬��ֱ����һ������������/������
	// ������Ȳ��ԣ���Ҫ����GL_DEPTH_TEST
	glEnable(GL_DEPTH_TEST);

	
	float mix_param = 0;
	float fov = 45.0f;
	float aspect_ratio = (float)ScreenWidth / ScreenHeight;

	glm::vec3 view_vec = glm::vec3(0, 0, -3.0f);

	// ���һ��whileѭ�������ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	// glfwWindowShouldClose����������ÿ��ѭ���Ŀ�ʼǰ���һ��GLFW�Ƿ�Ҫ���˳�������ǵĻ����ú�������true����Ⱦѭ����ֹͣ���У�֮�����ǾͿ��Թر�Ӧ�ó���
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		// glClearColor���������Ļ���õ���ɫ��������glClear�����������ɫ����֮��������ɫ���嶼�ᱻ���ΪglClearColor�������õ���ɫ
		glClearColor(0.2f, 0.3f, 0.3f, 0.9f);
		// glClear���������Ļ����ɫ���壬������һ������λ(Buffer Bit)��ָ��Ҫ��յĻ��壬���ܵĻ���λ��GL_COLOR_BUFFER_BIT��GL_DEPTH_BUFFER_BIT��GL_STENCIL_BUFFER_BIT
		// ÿ����Ⱦ����֮ǰ�����Ȼ��壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glClearColor������һ��״̬���ú�������glClear��������һ��״̬ʹ�õĺ�������ʹ���˵�ǰ��״̬����ȡӦ�����Ϊ����ɫ

		if (glfwGetKey(window, GLFW_KEY_Z))
			mix_param = std::min(mix_param + 0.01f, 1.0f);
		else if (glfwGetKey(window, GLFW_KEY_X))
			mix_param = std::max(mix_param - 0.01f, 0.0f);

		//if (glfwGetKey(window, GLFW_KEY_Q))
		//	fov = std::min(fov + 0.2f, 100.0f);
		//else if (glfwGetKey(window, GLFW_KEY_W))
		//	fov = std::max(fov - 0.2f, 0.0f);

		//if (glfwGetKey(window, GLFW_KEY_A))
		//	aspect_ratio = std::min(aspect_ratio + 0.01f, 3.0f);
		//else if (glfwGetKey(window, GLFW_KEY_S))
		//	aspect_ratio = std::max(aspect_ratio - 0.01f, 0.0f);

		//if (glfwGetKey(window, GLFW_KEY_UP))
		//	view_vec -= glm::vec3(0, 0.1f, 0);
		//else if (glfwGetKey(window, GLFW_KEY_DOWN))
		//	view_vec += glm::vec3(0, 0.1f, 0);
		//else if (glfwGetKey(window, GLFW_KEY_RIGHT))
		//	view_vec -= glm::vec3(0.1f, 0, 0);
		//else if (glfwGetKey(window, GLFW_KEY_LEFT))
		//	view_vec += glm::vec3(0.1f, 0, 0);
		//else if (glfwGetKey(window, GLFW_KEY_E))
		//	view_vec -= glm::vec3(0, 0, 0.1f);
		//else if (glfwGetKey(window, GLFW_KEY_R))
		//	view_vec += glm::vec3(0, 0, 0.1f);

		camera.Update();
		
#pragma region MVP
		// mvp
		
		glm::mat4 view = camera.GetView(glm::vec3(0, 0, 0));
		glm::mat4 projection = glm::mat4(1.0f);
		

		//���ĵ�һ������������fov��ֵ������ʾ������Ұ(Field of View)�����������˹۲�ռ�Ĵ�С��
		// �����Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
		// �ڶ������������˿�߱�: aspect-ratio�����ӿڵĿ���Ը����á�
		// �����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ��
		projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
#pragma endregion

#pragma region ����
		// һ�������λ��ֵͨ����Ϊһ������Ԫ(Texture Unit)
		// glActiveTexture��������Ԫ, ��������glBindTexture�������û�����������ǰ���������Ԫ
		glActiveTexture(GL_TEXTURE0);
		texture1.Bind();
		glActiveTexture(GL_TEXTURE1);
		texture2.Bind();
#pragma endregion

#pragma region Shader
		//float timeValue = glfwGetTime();
		//float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram->GetID(), "ourColor");
		shaderProgram->Use();

		shaderProgram->SetUniformInt("texture1", 0);
		shaderProgram->SetUniformInt("texture2", 1);

		shaderProgram->SetUniformFloat("mix_param", mix_param);

		
		shaderProgram->SetUniformMat4f("view", view);
		shaderProgram->SetUniformMat4f("projection", projection);

		//// ����һ��uniform֮ǰ�������ʹ��shader���򣨵���glUseProgram)����Ϊ�����ڵ�ǰ�������ɫ������������uniform��
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 0.0f);
#pragma endregion

		glBindVertexArray(VAO[0]);

		////glDrawArrays������һ�����������Ǵ�����Ƶ�OpenGLͼԪ�����͡��ڶ�������ָ���˶����������ʼ���������һ������ָ�����Ǵ�����ƶ��ٸ�����
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f); // ͨ���������������ģ�;������ǽ��ö�������任����������
			model = glm::translate(model, cubePositions[i]);
			if (i % 3 == 0)
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, (float)i, 0.5f));

			shaderProgram->SetUniformMat4f("model", model);
			model1->Draw();
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