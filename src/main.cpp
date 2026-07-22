#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "main.h"
#include "Window.h"
#include <vulkan/vulkan.h>
#include <src/VulkanContext.h>

#include <vector>

// TODO: physicalDevice and device are delcated but not created.
//	 Need physical device selection (vkEnumeratePhysicalDevices +
//	 suitability check_ and logical device creation before this
//	 context is usable for rendering. Also, cleanup VulkanContext. Yays

int main()
{
	WindowConfig window{};
	WindowSystem::Initialize(window);

	if (!window.handle) 
	{
		return -1;
	}

	VulkanContext context{};

	if (!vkInstance(context, window.handle))
	{
		std::cerr << "VULKAN INSTANCE FAILED AT RUNTIME" << std::endl;
		return -1;
	}

	if (!vkSurfaceKHR(context, window.handle))
	{
		std::cerr << "VULKAN SURFACE FAILED AT RUNTIME" << std::endl;
		return -1;
	}

	if (!vkPhysicalDevice(context, window.handle))
	{
		std::cerr << "VULKAN PHYSICALDEVICE FAILED AT RUNTIME" << std::endl;
		return -1;
	}

	if (!vkDevice(context, window.handle))
	{
		std::cerr << "VULKAN DEVICE FAILED AT RUNTIME" << std::endl;
		return -1;
	}
	
	std::cout << "SUCCESS WINDOW AND INSTANCE WERE CREATED" << std::endl;

	while (!window.shouldClose) 
	{
		WindowSystem::Update(window);
	}

	vkDestroyDevice(context.device, nullptr);
	vkDestroySurfaceKHR(context.instance, context.surface, nullptr);
	vkDestroyInstance(context.instance, nullptr);
	WindowSystem::Shutdown(window);

	return 0;
}
