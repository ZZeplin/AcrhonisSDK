#include <iostream>

#include <vulkan/vulkan.h>
#include <src/vkb/vk_swapchain.h>
#include <src/vkb/vk_device.h>
#include <src/vkb/vk_queuefamilies.h>
#include <algorithm>
#include <vector>
#include <cassert>

namespace vkb {
	bool CheckForCompatibility(InstanceRuntime& instRuntime, SurfaceRuntime& surfRuntime, PhysDeviceRuntime& deviceRuntime) {
		uint32_t deviceCount{ 0 };
		vkEnumeratePhysicalDevices(instRuntime.vk_instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> availableDevices(deviceCount);
		vkEnumeratePhysicalDevices(instRuntime.vk_instance, &deviceCount, availableDevices.data());

		/* Gets the device from avaialableDevices and compares the
		   propery extension names until it finds one that supports
		   the Vulkan swapchain extension */

		for (const auto& device : availableDevices) {
			uint32_t propertyCount{ 0 };
			vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, nullptr);

			std::vector<VkExtensionProperties> properties(propertyCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, properties.data());

			bool swapchainSupported{ false };

			for (const auto& props : properties) {
				if (strcmp(props.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
					swapchainSupported = true;
					break;
				}
			}

			if(swapchainSupported) {
				QueueFamilyIndices indices{ findQueueFamilies(device, surfRuntime.vk_surface) };

				if (indices.isComplete()) {
					deviceRuntime.vk_physicalDevice = device;
					return true;
				}
			}
		}

		return false;
	}

	void SwapchainQueries(SwapchainQueryInfo& queryInfo, SwapchainData& data, SurfaceRuntime& surfRuntime, PhysDeviceRuntime& deviceRuntime) {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceRuntime.vk_physicalDevice, surfRuntime.vk_surface, &queryInfo.capabilities);

		// Sets the recommended imageCount to the min + 1 to prevent suttering
		data.imageCount = queryInfo.capabilities.minImageCount + 1;
		
		if (queryInfo.capabilities.maxImageCount > 0 && data.imageCount > queryInfo.capabilities.maxImageCount) {
			data.imageCount = queryInfo.capabilities.maxImageCount;
		}

		// Setting up image format and color space for the GPU
		vkGetPhysicalDeviceSurfaceFormatsKHR(deviceRuntime.vk_physicalDevice, surfRuntime.vk_surface, &data.formatCount, nullptr);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(data.formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(deviceRuntime.vk_physicalDevice, surfRuntime.vk_surface, &data.formatCount, surfaceFormats.data());

		queryInfo.imageFormat = surfaceFormats[0].format;
		queryInfo.imageColorSpace = surfaceFormats[0].colorSpace;
		
		if (queryInfo.imageFormat == VK_FORMAT_B8G8R8A8_SRGB && queryInfo.imageColorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			// Already does what it needs
		}
		else {
			for (const auto& availableFormat : surfaceFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					queryInfo.imageFormat = availableFormat.format;
					queryInfo.imageColorSpace = availableFormat.colorSpace;
					break;
				}
			}
		}
		
		//  Queries for the present mode
		vkGetPhysicalDeviceSurfacePresentModesKHR(deviceRuntime.vk_physicalDevice, surfRuntime.vk_surface, &data.presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> presentModes(data.presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(deviceRuntime.vk_physicalDevice, surfRuntime.vk_surface, &data.presentModeCount, presentModes.data());

		bool vsyncEnabled{ true };

		// Finds the bset present mode
		if (vsyncEnabled) {
			queryInfo.bestMode = VK_PRESENT_MODE_FIFO_KHR;
			for (const auto& mode : presentModes) {
				if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
					queryInfo.bestMode = mode;
					break;
				}
			}
		}
		else {
			queryInfo.bestMode = VK_PRESENT_MODE_FIFO_KHR;
			for (const auto& mode : presentModes) {
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
					queryInfo.bestMode = mode;
					break;
				}
			}
		}

		if (queryInfo.bestMode == VK_PRESENT_MODE_FIFO_KHR) {
			for (const auto& mode : presentModes) {
				if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
					queryInfo.bestMode = mode;
					break;
				}
			}
		}

	}

	bool CreateSwapchain(VkDevice device, SwapchainQueryInfo& queryInfo, SwapchainData& data, SurfaceRuntime& surfRuntime, SwapchainRuntime& swapRuntime) {
		VkImageUsageFlags imageUsage{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT };

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.surface = surfRuntime.vk_surface;
		createInfo.minImageCount = data.imageCount;
		createInfo.imageFormat = queryInfo.imageFormat;
		createInfo.imageColorSpace = queryInfo.imageColorSpace;
		createInfo.imageExtent = queryInfo.capabilities.currentExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = imageUsage;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = queryInfo.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = queryInfo.bestMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapRuntime.swapchain) != VK_SUCCESS) {
			throw std::runtime_error("Swapchain creation was unsuccessful!");
		}

		return true;
	}

	void DestroySwapchain(VkDevice device, SwapchainData& data) {
		if (data.swapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(device, data.swapchain, nullptr);
			data.swapchain = VK_NULL_HANDLE;
		}
	}
}