#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace vkb {

	struct InstanceProperties {
		const char* appName{ "Not Decided" };
		uint32_t applicationVersion{ VK_MAKE_VERSION(1, 0, 0) };
		const char* engineName{ "No Engine" };
		uint32_t engineVersion{ VK_MAKE_VERSION(1,0 ,0) };
		uint32_t apiVersion{ VK_API_VERSION_1_4 };



		const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
		std::vector<const char*> extensions;
		
#ifdef NDEBUG
		const bool enableValidationLayers{ false };
#else
		const bool enableValidationLayers{ true };
#endif

	};

	class VulkanInstance {
	public:
		explicit VulkanInstance(InstanceProperties& props);
		~VulkanInstance();

		VkInstance GetHandle() const { return m_Instance; }
	private:
		VkInstance m_Instance{ VK_NULL_HANDLE };
	};

	bool CheckValidationLayerSupport(InstanceProperties& props);
}