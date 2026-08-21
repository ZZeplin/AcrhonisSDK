#include <iostream>
#include <src/vkb/vk_queuefamilies.h>
#include <vector>

namespace vkb {

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount{ 0 };
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i{ 0 }; i < queueFamilyCount; ++i) {
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport{ VK_FALSE };
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}
		}

		return indices;
	}

	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
		QueueFamilyIndices indices{ findQueueFamilies(device, surface) };

		return indices.graphicsFamily.has_value() && indices.presentFamily.has_value();
	}
}