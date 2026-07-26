#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <src/Window.h>
#include <Vulkan/vulkan.h>
#include <optional>
#include <vector>

struct VulkanContext {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

bool createInstance(VulkanContext& context, GLFWwindow* window);
bool vkSurfaceKHR(VulkanContext& context, GLFWwindow* window);
bool vkPickPhysicalDevice(VulkanContext& context, GLFWwindow* window);
bool isDeviceSuitable(VkPhysicalDevice device);
bool isQueueFamilyComplete(const QueueFamilyIndices& indices);
bool checkValidationLayerSupport(VulkanContext& context, GLFWwindow* winodw);

void createLogicalDevice(VulkanContext& context, GLFWwindow* window);
void cleanup(VulkanContext& context, GLFWwindow* window);

std::vector<const char*> getRequiredExtensions();