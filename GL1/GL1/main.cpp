//��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���
//GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL / gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <iostream>
#include "ShaderProgram.h"

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

ShaderProgram* CreateShaderProgram(int x)
{
	const char* vertexShaderSource = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource1 = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource2 = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(0.0f, 1f, 1f, 1.0);\n"
		"}\0";


	Shader vertex_shader = Shader(&vertexShaderSource, ShaderType::Vertex);
	Shader* fragment_shader = nullptr;
	if (x == 0)
		fragment_shader = new Shader(&fragmentShaderSource1, ShaderType::Fragment);
	else
		fragment_shader = new Shader(&fragmentShaderSource2, ShaderType::Fragment);

	vertex_shader.CheckCompileSuccess();
	fragment_shader->CheckCompileSuccess();

	ShaderProgram* shaderProgram = new  ShaderProgram();
	shaderProgram->AttachShader(vertex_shader);
	shaderProgram->AttachShader(*fragment_shader);
	shaderProgram->Link();

	return shaderProgram;
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

	ShaderProgram* shaderProgram0 = CreateShaderProgram(0);
	ShaderProgram* shaderProgram1 = CreateShaderProgram(1);
	
	float vertices_1[] = {
		0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, -0.5f, 0.0f
	};
	float vertices_2[] = {
		0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f
	};

	// ��������vertices���±�, ע��������0��ʼ
	unsigned int indices_1[] = {
		0, 1, 2 // ��һ��������
	};
	// ��������vertices���±�, ע��������0��ʼ
	unsigned int indices_2[] = {
		0, 1, 2 // ��һ��������
	};

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
	// glBufferData�������û���������ݸ��Ƶ���ǰ�󶨻���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);
	/*
		glVertexAttribPointer��������OpenGL����ν�����������
		��һ������ָ������Ҫ���õĶ�������, ��Ӧ������ɫ���е� layout (location == ?)
		�ڶ�������ָ���������ԵĴ�С
		����������ָ�����ݵ�����
		���ĸ��������������Ƿ�ϣ�����ݱ���׼�����������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮��
		�����������������(Stride)�������������������Ķ���������֮��ļ������˵���Ǵ�������Եڶ��γ��ֵĵط�����������0λ��֮���ж����ֽ�
		���һ��������ʾλ�������ڻ�������ʼλ�õ�ƫ������������void*��������Ҫ����ǿ������ת��

		ÿ���������Դ�һ��VBO������ڴ��л���������ݣ��������Ǵ��ĸ�VBO�������п����ж��VBO����ȡ����ͨ���ڵ���glVertexAttribPointerʱ�󶨵�GL_ARRAY_BUFFER��VBO������
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray�Զ�������λ��ֵ��Ϊ���������ö������ԣ���������Ĭ���ǽ��õ�
	glEnableVertexAttribArray(0);

	// ��EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	// ���������Ƶ�����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_2, GL_STATIC_DRAW);


	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ��EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	// ���������Ƶ�����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2, GL_STATIC_DRAW);

	// glBindVertexArray����0��ʾ���ǰ��VAO
	// ����һ��VAOʱ��֮ǰ�󶨵�VAO���Զ��������ͨ������Ҫ�ֶ����һ��VAO
	glBindVertexArray(0);

	
	// ���һ��whileѭ�������ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	// glfwWindowShouldClose����������ÿ��ѭ���Ŀ�ʼǰ���һ��GLFW�Ƿ�Ҫ���˳�������ǵĻ����ú�������true����Ⱦѭ����ֹͣ���У�֮�����ǾͿ��Թر�Ӧ�ó���
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		// glClearColor���������Ļ���õ���ɫ��������glClear�����������ɫ����֮��������ɫ���嶼�ᱻ���ΪglClearColor�������õ���ɫ
		glClearColor(0.2f, 0.3f, 0.3f, 0.9f);
		// glClear���������Ļ����ɫ���壬������һ������λ(Buffer Bit)��ָ��Ҫ��յĻ��壬���ܵĻ���λ��GL_COLOR_BUFFER_BIT��GL_DEPTH_BUFFER_BIT��GL_STENCIL_BUFFER_BIT
		glClear(GL_COLOR_BUFFER_BIT);
		// glClearColor������һ��״̬���ú�������glClear��������һ��״̬ʹ�õĺ�������ʹ���˵�ǰ��״̬����ȡӦ�����Ϊ����ɫ
		
		shaderProgram0->Use();
		glBindVertexArray(VAO[0]);

		////glDrawArrays������һ�����������Ǵ�����Ƶ�OpenGLͼԪ�����͡��ڶ�������ָ���˶����������ʼ���������һ������ָ�����Ǵ�����ƶ��ٸ�����
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// glDrawElements��һ������ָ���˻��Ƶ�ģʽ, �ڶ��������ǻ��ƶ���ĸ������������������������������͡����һ������ָ��EBO�е�ƫ����
		// glDrawElements�����ӵ�ǰ�󶨵�GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ������
		// �ڰ�VAOʱ���󶨵����һ��Ԫ�ػ���������洢ΪVAO��Ԫ�ػ���������Ȼ�󣬰󶨵�VAOҲ���Զ��󶨸�EBO
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);


		shaderProgram1->Use();
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

		// glfwSwapBuffers�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壩��������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ��
		glfwSwapBuffers(window);
		// glfwPollEvents���������û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����
		glfwPollEvents();
	}

	// ����Ⱦѭ��������������Ҫ��ȷ�ͷ�/ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}