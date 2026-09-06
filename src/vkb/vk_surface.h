#pragma once
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <src/vkb/vk_instance.h>

namespace vkb {
	struct SurfaceRuntime {
		VkSurfaceKHR vk_surface{ VK_NULL_HANDLE };
	};

	bool CreateSurface(SurfaceRuntime& surfRuntime, InstanceRuntime& instRuntime, GLFWwindow* window);
	void DestroySurface(SurfaceRuntime& surfRuntime, InstanceRuntime& instRuntime);
}