#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "main.h"
#include "Window.h"
#include <vulkan/vulkan.h>
#include <src/VulkanContext.h>

#include <vector>

int main() {
	WindowConfig window{};
	WindowSystem::Initialize(window);

	if (!window.handle) {
		return -1;
	}

	VulkanContext context{};
	
	std::cout << "SUCCESS WINDOW AND INSTANCE WERE CREATED" << std::endl;

	while (!window.shouldClose) {
		WindowSystem::Update(window);
	}

	WindowSystem::Shutdown(window);

	return 0;
}
