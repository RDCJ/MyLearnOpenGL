//请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
//GLAD的头文件包含了正确的OpenGL头文件（例如GL / gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD

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

	// 注册函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// glad初始化要在配置opengl context之后执行
	if (!InitGLAD()) return -1;

	
	float vertices[] = {
		0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f
	};

	// 顶点数组vertices的下标, 注意索引从0开始
	unsigned int indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3 // 第二个三角形
	};

	unsigned int VBO, VAO;

	// glGenBuffers函数用来生成缓冲区对象的名称, 第一个参数是要生成的缓冲对象的数量，第二个参数是用来存储缓冲对象名称的数组
	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData用来把用户定义的数据复制到当前绑定缓冲
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	/*
		glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据
		第一个参数指定我们要配置的顶点属性, 对应顶点着色器中的 layout (location == ?)
		第二个参数指定顶点属性的大小
		第三个参数指定数据的类型
		第四个参数定义我们是否希望数据被标准化。如果设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间
		第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔，简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节
		最后一个参数表示位置数据在缓冲中起始位置的偏移量，类型是void*，所以需要进行强制类型转换

		每个顶点属性从一个VBO管理的内存中获得它的数据，而具体是从哪个VBO（程序中可以有多个VBO）获取则是通过在调用glVertexAttribPointer时绑定到GL_ARRAY_BUFFER的VBO决定的
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的
	glEnableVertexAttribArray(0);

	// EBO是一个缓冲区，就像一个顶点缓冲区对象一样，它存储 OpenGL 用来决定要绘制哪些顶点的索引
	unsigned int EBO;
	// 创建元素缓冲对象
	glGenBuffers(1, &EBO);
	// 绑定EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 把索引复制到缓冲
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const char* vertexShaderSource = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);\n"
		"}\0";
	Shader vertex_shader = Shader(&vertexShaderSource, ShaderType::Vertex);
	Shader fragment_shader = Shader(&fragmentShaderSource, ShaderType::Fragment);

	vertex_shader.CheckCompileSuccess();
	fragment_shader.CheckCompileSuccess();

	ShaderProgram shaderProgram = ShaderProgram();
	shaderProgram.AttachShader(vertex_shader);
	shaderProgram.AttachShader(fragment_shader);
	shaderProgram.Link();

	// glBindVertexArray传入0表示解绑当前的VAO
	// 当绑定一个VAO时，之前绑定的VAO会自动解绑，所以通常不需要手动解绑一个VAO
	glBindVertexArray(0);
	
	// 添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	// glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话，该函数返回true，渲染循环将停止运行，之后我们就可以关闭应用程序
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		// glClearColor设置清空屏幕所用的颜色。当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
		glClearColor(0.2f, 0.3f, 0.3f, 0.9f);
		// glClear函数清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲，可能的缓冲位有GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT
		glClear(GL_COLOR_BUFFER_BIT);
		// glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
		
		shaderProgram.Use();
		glBindVertexArray(VAO);

		/*
		//glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。第二个参数指定了顶点数组的起始索引。最后一个参数指定我们打算绘制多少个顶点
		glDrawArrays(GL_TRIANGLES, 0, 3);
		*/

		// glDrawElements第一个参数指定了绘制的模式, 第二个参数是绘制顶点的个数。第三个参数是索引的数据类型。最后一个参数指定EBO中的偏移量
		// glDrawElements函数从当前绑定到GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取其索引
		// 在绑定VAO时，绑定的最后一个元素缓冲区对象存储为VAO的元素缓冲区对象。然后，绑定到VAO也会自动绑定该EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
		// glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
		glfwPollEvents();
	}

	// 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}