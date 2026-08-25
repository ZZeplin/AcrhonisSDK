#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string_view>
#include <cstdint>

struct WindowConfig
{
	GLFWwindow* handle{ nullptr };
	int32_t width{ 800 };
	int32_t height{ 600 };
	std::string_view title{ "Achronis" };
	bool shouldClose{ false };
};

namespace WindowSystem 
{
	void Initialize(WindowConfig& window);
	void Update(WindowConfig& window);
	void Shutdown(WindowConfig& window);
}