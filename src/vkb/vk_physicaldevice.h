#pragma once
#include <vulkan/vulkan.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_surface.h>

namespace vkb {
	struct PhysDeviceRuntime {
		VkPhysicalDevice vk_physicalDevice{ VK_NULL_HANDLE };
	};

	void GetPhysicalDevice(InstanceRuntime& instRuntime, PhysDeviceRuntime& deviceRuntime, SurfaceRuntime& surfRuntime);
}