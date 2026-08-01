#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string_view>

struct WindowConfig
{
	GLFWwindow* handle{ nullptr };
	int width{ 800 };
	int height{ 600 };
	std::string_view title{ "Achronis" };
	bool shouldClose{ false };
};

namespace WindowSystem 
{
	void Initialize(WindowConfig& window);
	void Update(WindowConfig& window);
	void Shutdown(WindowConfig& window);
}