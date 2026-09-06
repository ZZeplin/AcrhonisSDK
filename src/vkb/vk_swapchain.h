#pragma once

#include <iostream>
#include <vulkan/vulkan.h>
#include <src/vkb/vk_device.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_physicaldevice.h>
#include <src/vkb/vk_surface.h>
#include <array>
#include <vector>

namespace vkb {
	struct SwapchainQueryInfo {
		VkFormat imageFormat;
		VkColorSpaceKHR imageColorSpace;
		VkPresentModeKHR bestMode{ VK_PRESENT_MODE_FIFO_KHR };
		VkSurfaceCapabilitiesKHR capabilities;
		int32_t highestPriority{ 0 };
	};

	struct SwapchainData {
		uint32_t imageCount{ 3 };
		uint32_t formatCount{ 0 };
		uint32_t presentModeCount{ 0 };
		VkSwapchainKHR swapchain;
	};

	struct SwapchainRuntime {
		VkSwapchainKHR swapchain;
	};

	void SwapchainQueries(SwapchainQueryInfo& queryInfo, SwapchainData& data, SurfaceRuntime& surfRuntime, PhysDeviceRuntime& deviceRuntime);
	bool CreateSwapchain(VkDevice device, SwapchainQueryInfo& queryInfo, SwapchainData& data, SurfaceRuntime& surfRuntime, SwapchainRuntime& swapRuntime);
	bool CheckForCompatibility(InstanceRuntime& instRuntime, SurfaceRuntime& surfRuntime, PhysDeviceRuntime& deviceRuntime);
	void DestroySwapchain(VkDevice device, SwapchainRuntime& swapRuntime);
}