#include <src/vkb/vk_instance.h>
#include <vector>
#include <cstring>
#include <stdexcept >


namespace vkb {

	bool CheckValidationLayerSupport(InstanceProperties& props) {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// loops in range validationLayers for each required layerName. layerFound acts
		// as a flag -- it starts as fals, and only swaps to true if strcmp finds an
		// exact match, at which point it stops searching for that layer and moves on
		// to the next. If you are unfamiliar with strcmp -- it derives from the C 
		// standard library meaning "string compare", it takes 2 strings and compares
		// them character by character. In this case, it's comparing 2 string until
		// they are exactly equal.

		// Note for newer programmers: this isn't the traidtion 3-part for loop you may
		// have learned first. This is a range-based for loop -- it automatically walks
		// through every element in validationLayers, so there's no separate condition
		// or increment to write. The language handles that for you.

		for (const char* layerName : props.validationLayers) {
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

	VulkanInstance::VulkanInstance(InstanceProperties& props) {
		if (props.enableValidationLayers && !CheckValidationLayerSupport(props)) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = props.appName;
		appInfo.applicationVersion = props.applicationVersion;
		appInfo.pEngineName = props.engineName;
		appInfo.engineVersion = props.engineVersion;
		appInfo.apiVersion = props.apiVersion;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(props.extensions.size());
		createInfo.ppEnabledExtensionNames = props.extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(props.validationLayers.size());
		createInfo.ppEnabledLayerNames = props.validationLayers.data();

		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
			throw std::runtime_error("Vulkan Instance was not created (how is this even possible)!");
		}
	}

	VulkanInstance::~VulkanInstance() {
		if (m_Instance != VK_NULL_HANDLE) {
			vkDestroyInstance(m_Instance, nullptr);
			m_Instance = VK_NULL_HANDLE;
		}
	}
}