#include <iostream>
#include <src/vkb/vk_physicaldevice.h>
#include <src/vkb/vk_queuefamilies.h>
#include <vector>
#include <stdexcept>

namespace vkb {

	PhysicalDevice::PhysicalDevice(VulkanInstance& instance, VkSurfaceKHR& surface) : m_instance(instance), m_surface(surface), m_physicalDevice(VK_NULL_HANDLE) {
		uint32_t deviceCount{ 0 };
		if (vkEnumeratePhysicalDevices(m_instance.GetHandle(), &deviceCount, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("Enumerate physical devices failed to succeed!");
		}

		if (deviceCount == 0) {
			throw std::runtime_error("Failed to find GPU with vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		if (vkEnumeratePhysicalDevices(m_instance.GetHandle(), &deviceCount, devices.data()) != VK_SUCCESS) {
			throw std::runtime_error("Enumerate physical devices failed to succeed~");
		}

		for (const auto& device : devices) {
			if (isDeviceSuitable(device, m_surface)) {
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find a suitable device!");
		}
	}
}