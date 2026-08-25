#pragma once
#include <vulkan/vulkan.h>

namespace vkb {
	class VulkanDevice {
	public:
		explicit VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		~VulkanDevice();

		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator = (const VulkanDevice&) = delete;

		VkDevice GetHandle() const { return m_device; }
	private:
		VkDevice m_device{ VK_NULL_HANDLE };
	};
}