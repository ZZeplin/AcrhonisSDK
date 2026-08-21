#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "main.h"
#include "Window.h"
#include <vulkan/vulkan.h>
#include <src/VulkanContext.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_surface.h>

#include <vector>

int main() {
	WindowConfig window{};
	WindowSystem::Initialize(window);

	if (!window.handle) {
		return -1;
	}

	uint32_t glfwExtensionCount{ 0 };
	const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

	try {
		vkb::InstanceProperties props{};
		props.extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

		vkb::VulkanInstance instance(props);
		vkb::SurfaceKHR surface(instance, window.handle);

		std::cout << "SUCCESS WINDOW AND INSTANCE WERE CREATED" << std::endl;
  
		while (!window.shouldClose) {
			WindowSystem::Update(window);
		}
	}
	catch (const std::exception& e){
		std::cerr << "Fatal Error: " << e.what() << std::endl;
		WindowSystem::Shutdown(window);
		return -1;
	}

	WindowSystem::Shutdown(window);

	return 0;
}
