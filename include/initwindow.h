#ifndef INITWINDOW_H
#define INITWINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Window
{
    private:
		unsigned int WINDOW_WIDTH = 1200;
		unsigned int WINDOW_HEIGHT = 1600;
		GLFWwindow* window = nullptr;
		void initGlfw();
		void initGlad();
		void initViewPoint();
		void framebuffer_size_callback();
		void createWindow();
	public:
		Window();
		GLFWwindow* returnwindow();
};

GLFWwindow* Window::returnwindow()
{
	return window;
}

//初始化glfw
void Window::initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

//初始化glad
void Window::initGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
}

//初始化视口
void Window::initViewPoint()
{
	//视口大小，左下角位置与宽高
	glViewport(0, 0, 1600, 1200);
}

//创建窗口
void Window::createWindow()
{
	window = glfwCreateWindow(1600, 1200, "The world is based on Hatsune Miku", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	//通知GLFW将窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);
}

//当用户改变窗口的大小的时候，视口也被调整
void Window::framebuffer_size_callback()
{
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

Window::Window() : window(nullptr)
{
	initGlfw();
	createWindow();
	glfwMakeContextCurrent(window);
	initGlad();
}

#endif