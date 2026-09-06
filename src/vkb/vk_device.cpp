#include <iostream>
#include <vulkan/vulkan.h>

#include <src/vkb/vk_device.h>
#include <src/vkb/vk_queuefamilies.h>
#include <src/vkb/vk_physicaldevice.h>
#include <src/vkb/vk_swapchain.h>
#include <cstring>
#include <vector>
#include <set>
#include <string>

namespace vkb {
	bool CheckSwapChainSupport(DeviceProperties props, VkPhysicalDevice physicalDevice) {
		uint32_t deviceExtensionCount{ 0 };
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availableExtensions.data());

		for (const char* required : props.requiredDeviceExtensions) {
			bool foundExtension{ false };

			for (const auto& extension : availableExtensions) {
				if (std::strcmp(required, extension.extensionName) == 0) {
					foundExtension = true;
					break;
				}
			}

			if (!foundExtension) {
				return false;
			}
		}

		return true;
	}

	VulkanDevice::VulkanDevice(DeviceProperties props, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
		vkb::QueueFamilyIndices indices{ vkb::findQueueFamilies(physicalDevice, surface) };

		uint32_t uniqueQueueFamilies[2];
		uint32_t uniqueCount{ 0 };

		uniqueQueueFamilies[uniqueCount++] = indices.graphicsFamily.value();

		if (indices.presentFamily.value() != indices.graphicsFamily.value()) {
			uniqueQueueFamilies[uniqueCount++] = indices.presentFamily.value();
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(static_cast<size_t>(uniqueCount));
		float queuePriority{ 1.0f };

		for (uint32_t i{ 0 }; i < uniqueCount; ++i) {
			queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[i].queueFamilyIndex = uniqueQueueFamilies[i];
			queueCreateInfos[i].queueCount = 1;
			queueCreateInfos[i].pQueuePriorities = &queuePriority;
		}

		//Activates the raw GPU pointer features from vulkan 1.2
		VkPhysicalDeviceVulkan12Features features12{};
		features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.bufferDeviceAddress = VK_TRUE;
		features12.pNext = nullptr;

		// Sets up the vulkan 1.4 feature structure
		VkPhysicalDeviceVulkan14Features features14{};
		features14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
		features14.pNext = &features12;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = props.requiredDeviceExtensions.size();
		createInfo.ppEnabledExtensionNames = props.requiredDeviceExtensions.data();
		createInfo.pNext = &features14;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create device");
		}

		std::cout << "THIS RAN JUST CHECKING\n";
	}

	VulkanDevice::~VulkanDevice() {
		if (m_device != VK_NULL_HANDLE) {
			vkDestroyDevice(m_device, nullptr);
			m_device = VK_NULL_HANDLE;
		}
	}
}