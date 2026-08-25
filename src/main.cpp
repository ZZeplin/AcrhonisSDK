#include "main.h"
#include "Window.h"
#include <vulkan/vulkan.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_surface.h>
#include <src/vkb/vk_device.h>
#include <src/vkb/vk_physicaldevice.h>

#include <iostream>
#include <vector>
#include <array>
#include <cassert>

int main() {
	WindowConfig window{};
	WindowSystem::Initialize(window);

	if (!window.handle) {
		return -1;
	}

	vkb::InstanceProperties props{};

	uint32_t glfwExtensionCount{ 0 };
	const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };
	for (uint32_t i{ 0 }; i < glfwExtensionCount; ++i) {
		assert(props.extensionCount < props.extensions.size());
		props.extensions[props.extensionCount++] = glfwExtensions[i];
	}

	try {
		vkb::VulkanInstance instance(props);
		vkb::SurfaceKHR surface(instance, window.handle);
		vkb::PhysicalDevice physicalDevice(instance, surface.GetHandle());
		vkb::VulkanDevice device(physicalDevice.GetHandle(), surface.GetHandle());

		std::cout << "SUCCESS WINDOW, INSTANCE, AND DEVICE WERE CREATED!!!!!\n";
  
		while (!window.shouldClose) {
			WindowSystem::Update(window);
		}
	}
	catch (const std::exception& e){
		std::cerr << "Fatal Error: " << e.what() << "\n";
		WindowSystem::Shutdown(window);
		return -1;
	}

	WindowSystem::Shutdown(window);

	return 0;
}
