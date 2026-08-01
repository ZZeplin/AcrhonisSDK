#pragma once
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <src/vkb/vk_instance.h>

namespace vkb {

	class SurfaceKHR {
	public:
		explicit SurfaceKHR(VulkanInstance& instance, GLFWwindow* window);
		~SurfaceKHR();

		VkSurfaceKHR GetHandle() const { return m_surface; }
	private:
		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

		VulkanInstance& m_Instance;
	};
}