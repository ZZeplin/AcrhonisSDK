#include "main.h"
#include "Window.h"
#include <vulkan/vulkan.h>
#include <src/vkb/vk_instance.h>
#include <src/vkb/vk_surface.h>
#include <src/vkb/vk_device.h>
#include <src/vkb/vk_physicaldevice.h>
#include "src/vkb/vk_swapchain.h"

#include <iostream>
#include <vector>
#include <array>
#include <cassert>

namespace vkb{
void createSwapChain(SwapChainProperties& props, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, uint32_t windowWidth, uint32_t windowHeight);
void destroySwapChain(SwapChainProperties& props, VkDevice device);
};

void recreateSwapChain(vkb::SwapChainProperties& swapChainProps, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, WindowConfig& window) {
	while (window.width == 0 || window.height == 0) {
		WindowSystem::Update(window);
	}

	vkDeviceWaitIdle(device);

	vkb::destroySwapChain(swapChainProps, device);

	vkb::createSwapChain(swapChainProps, physicalDevice, device, surface, window.width, window.height);

	std::cout << "SWAP CHAIN RECREATED SUCCESSFULLY TO: " << window.width << "x" << window.height << "\n";
}

int main() {
	WindowConfig window{};
	WindowSystem::Initialize(window);

	if (!window.handle) {
		return -1;
	}

	vkb::InstanceProperties props{};
	vkb::DeviceProperties deviceProps{};

	uint32_t glfwExtensionCount{ 0 };
	const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };
	for (uint32_t i{ 0 }; i < glfwExtensionCount; ++i) {
		assert(props.extensionCount < props.extensions.size());
		props.extensions[props.extensionCount++] = glfwExtensions[i];
	}

	try {
		vkb::VulkanInstance instance(props);
		vkb::SurfaceKHR surface(instance, window.handle);
		vkb::PhysicalDevice physicalDevice(instance, surface.GetHandle());
		vkb::VulkanDevice device(deviceProps, physicalDevice.GetHandle(), surface.GetHandle());

		vkb::SwapChainProperties swapChainProps{};

		std::cout << "SUCCESS WINDOW, INSTANCE, AND DEVICE WERE CREATED!!!!!\n";

		createSwapChain(swapChainProps, physicalDevice.GetHandle(), device.GetHandle(), surface.GetHandle(), window.width, window.height);
  
		if (swapChainProps.swapChain != VK_NULL_HANDLE) {
			std::cout << "-> Handle allocated: " << swapChainProps.swapChain << "\n";
			std::cout << "-> Images created:    " << swapChainProps.swapChainImages.size() << " buffers allocated by the GPU\n";
			std::cout << "-> Image Format enum: " << swapChainProps.swapChainImageFormat << "\n";
			std::cout << "-> Window Resolution: " << swapChainProps.swapChainExtent.width << "x" << swapChainProps.swapChainExtent.height << "\n";
		}
		else {
			std::cout << "CRITICAL ERROR: Swap chain creation returned a null handle!\n";
		}

		glfwGetFramebufferSize(window.handle, &window.width, &window.height);

		int lastWidth = window.width;
		int lastHeight = window.height;

		while (!window.shouldClose) {
			WindowSystem::Update(window);

			glfwGetFramebufferSize(window.handle, &window.width, &window.height);

			if (window.width != lastWidth || window.height != lastHeight) {

				recreateSwapChain(swapChainProps, physicalDevice.GetHandle(), device.GetHandle(), surface.GetHandle(), window);
				lastWidth = window.width;
				lastHeight = window.height;
			}
		}
		vkDeviceWaitIdle(device.GetHandle());

		vkb::destroySwapChain(swapChainProps, device.GetHandle());
	}
	catch (const std::exception& e){
		std::cerr << "Fatal Error: " << e.what() << "\n";
		WindowSystem::Shutdown(window);
		return -1;
	}

	WindowSystem::Shutdown(window);

	return 0;
}
