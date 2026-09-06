#include <iostream>
#include <src/vkb/vk_physicaldevice.h>
#include <src/vkb/vk_queuefamilies.h>
#include <vector>
#include <stdexcept>

namespace vkb {

	void GetPhysicalDevice(InstanceRuntime& instRuntime, PhysDeviceRuntime& deviceRuntime, SurfaceRuntime& surfRuntime) {
		uint32_t deviceCount{ 0 };
		vkEnumeratePhysicalDevices(instRuntime.vk_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Failed to find a GPU with vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instRuntime.vk_instance, &deviceCount, devices.data());

		/* Evaluates each GPU and checks its compatibility with the surface -- 
		   if the GPU is compatible, then it set that GPU to our physicalDevice */
		for (const auto& device : devices) {
			if (isDeviceSuitable(device, surfRuntime.vk_surface)) {
				deviceRuntime.vk_physicalDevice = device;
				break;
			}
		}

		if (deviceRuntime.vk_physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find a suitable device!");
		}
	}
}