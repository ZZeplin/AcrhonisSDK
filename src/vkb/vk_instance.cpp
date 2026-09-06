#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <src/vkb/vk_instance.h>
#include <stdexcept>


namespace vkb {

	bool CheckValidationLayerSupport(const InstanceCreateInfo& instCreateInfo) {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Getting the original layerName from requiredLayers then getting
		// the availableLayer from availableLayers. After which, then compares
		// laeryName to the availableLayer to see if they are exactly equal,
		// if they are it then sets the foundLayer to true and ends the loop,
		// if not, returns false.
		for (const auto& layerName : instCreateInfo.requiredLayers) {
			bool foundLayer{ false };

			for (const auto& availableLayer : availableLayers) {
				if (strcmp(layerName, availableLayer.layerName) == 0) {
					foundLayer = true;
					break;
				}
			}

			if (!foundLayer) {
				return false;
			}
		}
	
		return true;
	}

	void GetRequiredExtensions(InstanceCreateInfo& instCreateInfo) {
		uint32_t glfwExtensionCount{ 0 };
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

		instCreateInfo.requiredExtensions.reserve(instCreateInfo.requiredExtensions.size() + glfwExtensionCount);

		for (uint32_t i{ 0 }; i < glfwExtensionCount; ++i) {
			instCreateInfo.requiredExtensions.push_back(glfwExtensions[i]);
		}
	}

	bool CreateInstance(const InstanceCreateInfo& instCreateInfo, InstanceRuntime& instRuntime) {
		// Checking if the requested validation layers are actually available on
		// this system before proceeding. If they aren't, fail with a clear error
		// before the instance is created to prevent a non-obvious cause.
		if (instCreateInfo.enableValidationLayers && !CheckValidationLayerSupport(instCreateInfo)) {
			throw std::runtime_error("The requested validation layer is not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = instCreateInfo.appName;
		appInfo.applicationVersion = instCreateInfo.applicationVersion;
		appInfo.pEngineName = instCreateInfo.engineName;
		appInfo.engineVersion = instCreateInfo.engineVersion;
		appInfo.apiVersion = instCreateInfo.apiVersion;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		// If validation layers are enabled, pass them to instanceCreateInfo;
		// If not, then leave enabledLayercount && ppEnabledLayerNames are their defaults
		if (instCreateInfo.enableValidationLayers) {
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instCreateInfo.requiredLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = instCreateInfo.requiredLayers.data();
		}
		else {
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.ppEnabledLayerNames = nullptr;
		}

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instCreateInfo.requiredExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instCreateInfo.requiredExtensions.data();

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &instRuntime.vk_instance) != VK_SUCCESS) {
			throw std::runtime_error("VkCreateInstance failed to succeed!");
		}

		return true;
	}

	// Destroys the instance, pretty self explanatory, but, just wanted to make sure you knew! :)
	void DestroyInstance(InstanceRuntime& instRuntime) {
		if (instRuntime.vk_instance != VK_NULL_HANDLE) {
			vkDestroyInstance(instRuntime.vk_instance, nullptr);
			instRuntime.vk_instance = VK_NULL_HANDLE;
		}
	}
}