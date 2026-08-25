#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <array>

namespace vkb {

	struct InstanceProperties {
		const char* appName{ "Not Decided" };
		uint32_t applicationVersion{ VK_MAKE_VERSION(1, 0, 0) };
		const char* engineName{ "No Engine" };
		uint32_t engineVersion{ VK_MAKE_VERSION(1,0 ,0) };
		uint32_t apiVersion{ VK_API_VERSION_1_4 };

		std::array<const char*, 4> validationLayers{ "VK_LAYER_KHRONOS_validation" };
		uint32_t validationLayerCount{ 1 };

		std::array<const char*, 16> extensions{};
		uint32_t extensionCount{ 0 };
		
#ifdef NDEBUG
		const bool enableValidationLayers{ false };
#else
		const bool enableValidationLayers{ true };
#endif

	};

	class VulkanInstance {
	public:
		explicit VulkanInstance(const InstanceProperties& props);
		~VulkanInstance();

		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance& operator = (const VulkanInstance&) = delete;

		VkInstance GetHandle() const { return m_Instance; }
	private:
		VkInstance m_Instance{ VK_NULL_HANDLE };
	};

	bool CheckValidationLayerSupport(const InstanceProperties& props);
}