#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vkb {
	struct DeviceProperties {
		std::vector<const char*> requiredDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

	bool CheckSwapChainSupport(DeviceProperties props, VkPhysicalDevice physicalDevice);

	class VulkanDevice {
	public:
		explicit VulkanDevice(DeviceProperties props, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		~VulkanDevice();

		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator = (const VulkanDevice&) = delete;

		VkDevice GetHandle() const { return m_device; }
	private:
		VkDevice m_device{ VK_NULL_HANDLE };
	};
}