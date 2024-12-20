#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
//变换使用的相关库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//使用文件图像加载库
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
//字符化顶点着色器代码,规定具体的顶点元素格式，包含顶点，颜色和纹理坐标
const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;\n" // 添加 uniform 变量来传递模型矩阵
"void main()\n"
"{\n"
"   gl_Position = model * vec4(aPos, 1.0);\n"  // 通过模型矩阵变换顶点坐标
"   ourColor = aColor;\n"
"   TexCoord = aTexCoord;\n"
"}\n\0";
//字符化片段着色器代码，使用GLSL内建的texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标。
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"FragColor = texture(ourTexture, TexCoord);\n"
"}\n\0";
//矩形
float vertices[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.2f,  0.2f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		 0.2f, -0.2f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.2f, -0.2f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.2f,  0.2f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
float vertices1[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.07f,  0.2f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		 0.07f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.07f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.07f,  0.2f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
float vertices2[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.07f,  0.2f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		 0.07f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.07f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.07f,  0.2f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
//EBO索引,利用VBO可以少设置节点
unsigned int indices1[] = {
	// 注意索引从0开始! 
	// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
	// 这样可以由下标代表顶点组合成矩形
	0, 1, 2, // 第一个三角形
	0, 2, 3, //第二个三角形
};
//配置视窗，使用回调函数来实现实时设置视窗
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
};
//绑定esc作为退出按键
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
};
int main() {
	//初始化
	glfwInit();
	//设置openGL的版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//无需使用扩展功能
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建GLFW窗口
	GLFWwindow* window = glfwCreateWindow(800, 600, "test002", NULL, NULL);
	if (window == NULL) {
		cout << "创造GLFW窗口失败！" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//配置视窗，使用回调函数来实现实时设置视窗
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//配置glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//glfwGetProcAddress是glfw库提供指针地址的函数
	{
		cout << "GLAD加载失败" << endl;
		return -1;
	}
	//配置顶点着色器对象
	//用unsign int定义一个着色器对象，然后把需要创建的着色器
	//类型以参数形式提供给glCreateShader。由于我们正在创建一
	//个顶点着色器，传递的参数是GL_VERTEX_SHADER。
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//把这个着色器源码附加到着色器对象上，然后编译它
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//同样方式配置片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//整合着色器

	//照例设置一个对象
	unsigned int shaderProgram;
	//glCreateProgram函数创建一个程序，并返回新创建程序对象
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//删除两个分立的着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//声明VAO，VBO
	unsigned int VBO[3], VAO[3], EBO[3];
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);
	//创建一个VAO
	// ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
	// 1. 绑定VAO
	glBindVertexArray(VAO[0]);
	// 2. 把顶点数组复制到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	// 4. 设置顶点属性指针,分别绑定纹理
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//进行纹理绑定
	unsigned int texture[3];
	glGenTextures(3, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 加载并生成纹理
	int width, height, nrChannels;
	//翻转Y轴
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data1 = stbi_load("mla001.jpg", &width, &height, &nrChannels, 0);
	//朴实无华的报错警告
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data1);
	// ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
    // 1. 绑定VAO
	glBindVertexArray(VAO[1]);
	// 2. 把顶点数组复制到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	// 4. 设置顶点属性指针,分别绑定纹理
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//进行纹理绑定
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//翻转Y轴
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data2 = stbi_load("mla002.png", &width, &height, &nrChannels, 0);
	//朴实无华的报错警告
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);
	//建立渲染循环部分
	// 1. 绑定VAO
	glBindVertexArray(VAO[2]);
	// 2. 把顶点数组复制到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	// 4. 设置顶点属性指针,分别绑定纹理
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//进行纹理绑定
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//翻转Y轴
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data3 = stbi_load("mla004.png", &width, &height, &nrChannels, 0);
	//朴实无华的报错警告
	if (data3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);
	//建立渲染循环部分
	glm::mat4 model = glm::mat4(1.0f);
	int fk=0;
	while (!glfwWindowShouldClose(window)) {//检查窗口是否被关闭
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glBindVertexArray(VAO[1]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else if(glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS&& glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS){
			glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glBindVertexArray(VAO[0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		// 检查按键输入
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			// 向左移动：平移矩阵
			model = glm::translate(model, glm::vec3(0.0005f, 0.0f, 0.0f));
			if (fk != 1&&fk !=0) {
				model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
				fk = 1;
			}
			glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glBindVertexArray(VAO[2]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			fk = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			// 向右移动：平移矩阵
			model = glm::translate(model, glm::vec3(0.0005f, 0.0f, 0.0f));
			if (fk != 2 && fk != 0) {
				model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
				fk = 2;
			}
			glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glBindVertexArray(VAO[2]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			fk = 2;
		}
		// 将模型矩阵传递给顶点着色器
		int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glfwSwapBuffers(window);//交换两帧的颜色缓冲
		glfwPollEvents();//响应事件
	}
	//释放VAO和VBO
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();//释放资源
	return 0;
}