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

//��ʼ��glfw
void Window::initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

//��ʼ��glad
void Window::initGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
}

//��ʼ���ӿ�
void Window::initViewPoint()
{
	//�ӿڴ�С�����½�λ�������
	glViewport(0, 0, 1600, 1200);
}

//��������
void Window::createWindow()
{
	window = glfwCreateWindow(1600, 1200, "FLEE", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	//֪ͨGLFW�����ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);
}

//���û��ı䴰�ڵĴ�С��ʱ���ӿ�Ҳ������
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