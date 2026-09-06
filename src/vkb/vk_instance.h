#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <array>
#include <vector>

namespace vkb {

	// Struct designed to be destroyed after its initial use
	struct InstanceCreateInfo {
		const char* appName{ "El Game" };
		const char* engineName{ "Achronis" };
		uint32_t apiVersion{ VK_API_VERSION_1_4 };
		uint32_t engineVersion{ VK_MAKE_VERSION(1, 0, 0) };
		uint32_t applicationVersion{ VK_MAKE_VERSION(1, 0, 0) };

		std::vector<const char*> requiredExtensions{};
		std::vector<const char*> requiredLayers{ "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
		const bool enableValidationLayers{ false };
#else
		const bool enableValidationLayers{ true };
#endif
	};

	// Struct designed to run continually on the stack
	struct InstanceRuntime {
		VkInstance vk_instance{ VK_NULL_HANDLE };
	};

	bool CreateInstance(const InstanceCreateInfo& instCreateInfo, InstanceRuntime& instRuntime);
	bool CheckValidationLayerSupport(const InstanceCreateInfo& instCreateInfo);
	void GetRequiredExtensions(InstanceCreateInfo& instCreateInfo);
	void DestroyInstance(InstanceRuntime& instRuntime);
}