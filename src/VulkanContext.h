#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <src/Window.h>
#include <Vulkan/vulkan.h>

struct VulkanContext
{
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
};

bool vkInstance(VulkanContext& context, GLFWwindow* window);
bool vkSurfaceKHR(VulkanContext& context, GLFWwindow* window);
bool vkPhysicalDevice(VulkanContext& context, GLFWwindow* window);
bool vkDevice(VulkanContext& context, GLFWwindow* window);