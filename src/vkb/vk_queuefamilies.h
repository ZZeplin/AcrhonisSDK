#pragma once
#include <vulkan/vulkan.h>
#include <optional>

namespace vkb {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool isComplete();
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
}