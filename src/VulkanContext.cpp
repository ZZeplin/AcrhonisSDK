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
#include <optional>
#include <cstring>

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
const bool enableValidationLayers{ false };
#else
const bool enableValidationLayers{ true };
#endif

bool checkValidationLayerSupport(VulkanContext& context, GLFWwindow* window) {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// loops in the range validationLayers for each required layer name. layerFound
	// acts as a flag -- it starts false, and only swaps to true if strcmp finds an
	// exact match, at which point it stops searching for that layer and moves on
	// to the next. If you are unfamiliar with strcmp -- it derives from the C 
	// standard library meaning "string compare", it takes 2 strings and compares 
	// them character by character. In this case, it's comparing 2 strings until they
	// are exactly equal.

	// Note for newer programmers: this isn't the traditional 3-part for loop you may
	// have learned first. This is a range-based for loop -- it automatically walks
	// through every element in validationLayers, so there's no separate condition or 
	// increment to write. The language handles that for you.

	for (const char* layerName : validationLayers) {
		bool layerFound{ false };

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount{ 0 };
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//Not using this right now

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool createInstance(VulkanContext& context, GLFWwindow* window) {
	if (enableValidationLayers && !checkValidationLayerSupport(context, window)) {
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{
		.sType {VK_STRUCTURE_TYPE_APPLICATION_INFO},
		.pNext {nullptr},
		.pApplicationName {"Achronis"},
		.applicationVersion {VK_MAKE_VERSION(1, 0, 0)},
		.pEngineName {"No Engine"},
		.engineVersion {VK_MAKE_VERSION(1, 0, 0)},
		.apiVersion {VK_API_VERSION_1_4}
	};

	auto extensions{ getRequiredExtensions() };

	VkInstanceCreateInfo createInfo{
		.sType {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO},
		.pNext {nullptr},
		.flags {0},
		.pApplicationInfo {&appInfo},
		.enabledLayerCount {static_cast<uint32_t>(validationLayers.size())},
		.ppEnabledLayerNames {validationLayers.data()},
		.enabledExtensionCount {static_cast<uint32_t>(extensions.size())},
		.ppEnabledExtensionNames {extensions.data()}
	};

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult Instance = vkCreateInstance(&createInfo, nullptr, &context.instance);

	if (Instance != VK_SUCCESS) {
		throw std::runtime_error("Vulkan instance failed to be created!");
	}

	return true;
}

bool vkSurfaceKHR(VulkanContext& context, GLFWwindow* window) {
#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR vkWin32SurfInfo {
		.sType {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR},
		.pNext {nullptr},
		.flags {0},
		.hinstance {GetModuleHandle(nullptr)},
		.hwnd {glfwGetWin32Window(window)}
	};

	VkResult win32SurfResult {vkCreateWin32SurfaceKHR(context.instance, &vkWin32SurfInfo, nullptr, &context.surface)};

	if (win32SurfResult != VK_SUCCESS) {
		throw std::runtime_error("Win32 Surface did not succeed!");
	}

	// I am running on windows, I have no idea if this works -- I am dualbooting,
	// I'm just too lazy to actually test it on CachyOS. Worth noting this is also
	// not setup for X11, if plans are to run on X11, it's worth the implementation...
	// again, I'm too lazy -- after all, I am a programmer.

#elif defined(__linux__)
	VkWaylandSurfaceCreateInfoKHR vkWaylandSurfInfo {
		.sType {VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR},
		.pNext {nullptr},
		.flags {0},
		.display {glfwGetWaylandDisplay()},
		.surface {glfwGetWaylandWindow(window)}
	};

	VkResult waylandSurfResult {vkCreateWaylandSurfaceKHR(context.instance, &vkWaylandSurfInfo, nullptr, &context.surface)};

	if (waylandSurfResult != VK_SUCCESS) {
		throw std::runtime_error("Wayland Surface did not succeed!");
	}
#else
	#error "Unsupported Platform"
#endif

	return true;
}

// this is the most disgusting code I have ever seen, clean this shit up. 2:43am 7/22/26
// [Cleaned 1:14pm 7/23/26] literally just followed the official vulkan tutorial instead
// of trying to do everything off my mind, makes it so much easier, doesn't it?

// runs a check and returns true if graphicsFamily has been assigned a value (self
// explanatory if you know what std::optional does.)

bool isQueueFamilyComplete(const QueueFamilyIndices& indices) {
	return indices.graphicsFamily.has_value();
}

bool isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return isQueueFamilyComplete(indices);
}

bool vkPickPhysicalDevice(VulkanContext& context, GLFWwindow* window) {
	uint32_t deviceCount {0};
	VkResult vkEnumPhysDevices {vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr)};

	if (vkEnumPhysDevices != VK_SUCCESS) {
		throw std::runtime_error("EnumeratePhysicalDevices failed to succeed");
	}

	// if no device, then no work :)

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	// what the fuck is going on here?!?!?!? 10:02pm 7/22/26
	// [Fixed 1:14pm 7/23/26] tore it all down and now it just gets a device that
	// has Vulkan support now, instead of whatever was going on before...
	// I still have no idea what was going on before.

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(context.instance, &deviceCount, devices.data());

	// a for loop designed to loop through the range of devices to find a suitable
	// device. If you aren't familiar with this form of syntax: const is a safeguard
	// to prevent you from modifying device as an object itself, auto& means device 
	// is a reference to the actual element in devices on each iteration of the loop 
	// (not a copy), and the ':' is there to say "for each device in devices" or just
	// a way to say "look through each device in the range of devices until it finds
	// a suitable device".

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			context.physicalDevice = device;
			break;
		}
	}

	if (context.physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	return true;
}

// Now finding queueFamilies separate from a singular function, probably better
// this way to prevent any exceptionally convoluted functions that lack any
// real readability. 1:14pm 7/23/26

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount {0};
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// A for loop designed to loop through queues until it finds one that contains 
	// the Vulkan Queue Graphics Bit and adds a value to graphicsFamily. This is actually
	// the only reason std::optional is used for our uint32_t -- more so for safety 
	// purposes than anything else.

	int i {0};
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (isQueueFamilyComplete(indices)) {
			break;
		}

		i++;
	}

	return indices;
}

// Oh crud, who in their mind conjured up this monstrosity... oh wait, that's right, I did... shit. 2:51am 7/22/26
// [Fixed 2:54pm 7/23/26] no longer has two separate hand-written copies of the same queue-family-search loop -- 
// both functions now call findQueueFamilies() instead. Still technically runs the search twice at runtime (once 
// while picking the device, once again here), but that's a much smaller cost than duplicated logic. :)

void createLogicalDevice(VulkanContext& context, GLFWwindow* window) {
	QueueFamilyIndices indices {findQueueFamilies(context.physicalDevice)};

	VkDeviceQueueCreateInfo queueCreateInfo{
		.sType {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO},
		.queueFamilyIndex {indices.graphicsFamily.value()},
		.queueCount {1}
	};

	float queuePriority{ 1.0f };
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	// deliberately not requesting a VK_KHR_swapchain here yet. Not every Vulkan device
	// supports presenting to a window, so this gets added once we actually set up the 
	// swapchain later.

	VkDeviceCreateInfo createInfo {
		.sType {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO},
		.queueCreateInfoCount {1},
		.pQueueCreateInfos {&queueCreateInfo},
		.enabledExtensionCount {0},
		.pEnabledFeatures {&deviceFeatures},
	};

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult createDevice = vkCreateDevice(context.physicalDevice, &createInfo, nullptr, &context.device);

	if (createDevice != VK_SUCCESS) {
		throw std::runtime_error("Vulkan device failed to create!");
	}
	
	vkGetDeviceQueue(context.device, indices.graphicsFamily.value(), 0, &context.graphicsQueue);
}

void cleanup(VulkanContext& context, GLFWwindow* window) {
	vkDestroyDevice(context.device, nullptr);
	vkDestroySurfaceKHR(context.instance, context.surface, nullptr);
	vkDestroyInstance(context.instance, nullptr);
}