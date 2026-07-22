#include "Window.h"
#include <iostream>

void WindowSystem::Initialize(WindowConfig& window) 
{
	if (!glfwInit()) 
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window.handle = glfwCreateWindow(window.width, window.height, window.title.data(), nullptr, nullptr);
	if (!window.handle) 
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
}

void WindowSystem::Update(WindowConfig& window) 
{
	if (window.handle) 
	{
		glfwPollEvents();
		window.shouldClose = glfwWindowShouldClose(window.handle);
	}
}

void WindowSystem::Shutdown(WindowConfig& window) 
{
	if (window.handle) 
	{
		glfwDestroyWindow(window.handle);
	}

	glfwTerminate();
}