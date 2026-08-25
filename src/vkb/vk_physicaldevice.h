#pragma once
#include <vulkan/vulkan.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_surface.h>

namespace vkb {

	class PhysicalDevice {
	public:
		explicit PhysicalDevice(VulkanInstance& instance, VkSurfaceKHR surface);
		~PhysicalDevice() = default;

		VkPhysicalDevice GetHandle() const { return m_physicalDevice; }
	private:
		VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
		VulkanInstance& m_instance;
		VkSurfaceKHR& m_surface;
	};
}