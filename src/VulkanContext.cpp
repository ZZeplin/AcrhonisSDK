#include <iostream>

#define GLFW_INCLUDE_VULKAN

#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <windows.h>
#elif defined(__linux__)
	#define VK_USE_PLATFORM_WAYLAND_KHR
	#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <src/Window.h>
#include <src/VulkanContext.h>

#include <vector>

// TODO: Validation layers are currently unconditional and unverified.
//   - Gate behind a debug-only build flag (e.g. #ifdef NDEBUG / _DEBUG)
//     so release builds skip validation entirely.
//   - Call vkEnumerateInstanceLayerProperties() to confirm
//     "VK_LAYER_KHRONOS_validation" is actually available before
//     enabling it, and fail with a clearer error if it isn't
//     (currently a missing layer just surfaces as a generic
//     VK_ERROR_LAYER_NOT_PRESENT via vkCreateInstance).

bool vkInstance(VulkanContext& context, GLFWwindow* window)
{
	VkApplicationInfo vkAppInfo {
		.sType {VK_STRUCTURE_TYPE_APPLICATION_INFO},
		.pNext {nullptr},
		.pApplicationName {"Achronis"},
		.applicationVersion {VK_MAKE_VERSION(1, 0, 0)},
		.pEngineName {"No Engine"},
		.engineVersion {VK_MAKE_VERSION(1, 0, 0)},
		.apiVersion {VK_API_VERSION_1_4}
	};

	uint32_t glfwExtensionsCount {0};
	const char** glfwExtension {glfwGetRequiredInstanceExtensions(&glfwExtensionsCount)};

	const char* validationLayer[] {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo vkInstInfo {
		.sType {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO},
		.pNext {nullptr},
		.flags {0},
		.pApplicationInfo {&vkAppInfo},
		.enabledLayerCount {1},
		.ppEnabledLayerNames {validationLayer},
		.enabledExtensionCount {glfwExtensionsCount},
		.ppEnabledExtensionNames {glfwExtension}
	};

	VkResult instanceResult {vkCreateInstance(&vkInstInfo, nullptr, &context.instance)};

	if (instanceResult != VK_SUCCESS)
	{
		std::cerr << "VK INSTANCE IS FALSE" << std::endl;
		return false;
	}

	return true;
}

bool vkSurfaceKHR(VulkanContext& context, GLFWwindow* window)
{
#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR vkWin32SurfInfo {
		.sType {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR},
		.pNext {nullptr},
		.flags {0},
		.hinstance {GetModuleHandle(nullptr)},
		.hwnd {glfwGetWin32Window(window)}
	};

	VkResult win32SurfResult {vkCreateWin32SurfaceKHR(context.instance, &vkWin32SurfInfo, nullptr, &context.surface)};

	if (win32SurfResult != VK_SUCCESS)
	{
		std::cerr << "WIN32 SURFACE DID NOT SUCCEED" << std::endl;
		return false;
	}
#elif defined(__linux__)
	VkWaylandSurfaceCreateInfoKHR vkWaylandSurfInfo {
		.sType {VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR},
		.pNext {nullptr},
		.flags {0},
		.display {glfwGetWaylandDisplay()},
		.surface {glfwGetWaylandWindow(window)}
	};

	VkResult waylandSurfResult {vkCreateWaylandSurfaceKHR(context.instance, &vkWaylandSurfInfo, nullptr, &context.surface)};

	if (waylandSurfResult != VK_SUCCESS)
	{
		std::cerr << "WAYLAND SURFACE DID NOT SUCCEED" << std::endl;
		return false;
	}
#endif
	return true;
}

bool vkPhysicalDevice(VulkanContext& context, GLFWwindow* window)
{
	uint32_t deviceCount {0};
	VkResult vkEnumPhysDevices {vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr)};

	if (vkEnumPhysDevices != VK_SUCCESS)
	{
		return false;
	}

	std::vector<VkPhysicalDevice> tempHandles(deviceCount);
	vkEnumeratePhysicalDevices(context.instance, &deviceCount, tempHandles.data());

	std::vector<VkPhysicalDeviceProperties> tempProps(deviceCount);
	std::vector<uint32_t> tempQueueIndices(deviceCount, UINT32_MAX);
	std::vector<VkQueueFamilyProperties> queueFamilies;

	for (uint32_t d {0}; d < deviceCount; ++d)
	{
		VkPhysicalDevice device {tempHandles[d]};
		vkGetPhysicalDeviceProperties(device, &tempProps[d]);

		uint32_t queueFamilyCount {0};
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		queueFamilies.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (uint32_t q {0}; q < queueFamilyCount; ++q)
		{
			if (queueFamilies[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 presentSupport {VK_FALSE};

				vkGetPhysicalDeviceSurfaceSupportKHR(device, q, context.surface, &presentSupport);

				if (presentSupport)
				{
					tempQueueIndices[d] = q;
					break;
				}
			}
		}
	}

	uint32_t chosenIndex {UINT32_MAX};
	for (uint32_t i {0}; i < deviceCount; ++i)
	{
		if (tempQueueIndices[i] == UINT32_MAX) continue;

		if (tempProps[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			chosenIndex = i;
			break;
		}
	}

	if (chosenIndex == UINT32_MAX)
	{
		for (uint32_t i {0}; i < deviceCount; ++i)
		{
			if (tempQueueIndices[i] != UINT32_MAX)
			{
				chosenIndex = i;
				break;
			}
		}
	}

	if (chosenIndex != UINT32_MAX)
	{
		context.physicalDevice = tempHandles[chosenIndex];
		return true;
	}

	return false;
}

bool vkDevice(VulkanContext& context, GLFWwindow* window)
{
	if (context.physicalDevice == VK_NULL_HANDLE) return false;

	uint32_t queueFamilyCount {0};
	vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, queueFamilies.data());

	uint32_t queueFamilyIndex {UINT32_MAX};
	for (uint32_t q {0}; q < queueFamilyCount; ++q)
	{
		if (queueFamilies[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			VkBool32 presentSupport {VK_FALSE};
			vkGetPhysicalDeviceSurfaceSupportKHR(context.physicalDevice, q, context.surface, &presentSupport);
			if (presentSupport)
			{
				queueFamilyIndex = q;
				break;
			}
		}
	}

	if (queueFamilyIndex == UINT32_MAX) return false;

	float queuePriority {1.0f};
	VkDeviceQueueCreateInfo queueCreateInfo {
		.sType {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO},
		.queueFamilyIndex {queueFamilyIndex},
		.queueCount {1},
		.pQueuePriorities {&queuePriority}
	};

	const std::vector<const char*> deviceExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo {
		.sType {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO},
		.pNext {nullptr},
		.flags {0},
		.queueCreateInfoCount {1},
		.pQueueCreateInfos {&queueCreateInfo},
		.enabledLayerCount {0},
		.ppEnabledLayerNames {nullptr},
		.enabledExtensionCount {static_cast<uint32_t>(deviceExtensions.size())},
		.ppEnabledExtensionNames {deviceExtensions.data()},
		.pEnabledFeatures {&deviceFeatures}
	};

	VkResult createDevice {vkCreateDevice(context.physicalDevice, &deviceCreateInfo, nullptr, &context.device)};

	if (createDevice != VK_SUCCESS)
	{
		std::cerr << "VULKAN DEVICE DID NOT SUCCEED" << std::endl;
		return false;
	}

	return true;
}