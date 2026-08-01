#include <iostream>
#include <stdexcept>
#include <src/vkb/vk_surface.h>

namespace vkb {

	SurfaceKHR::SurfaceKHR(VulkanInstance& instance, GLFWwindow* window) : m_Instance(instance) {
		if (glfwCreateWindowSurface(m_Instance.GetHandle(), window, nullptr, &m_surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface! (How is this even possible)");
		}
	}

	SurfaceKHR::~SurfaceKHR() {
		if (m_surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(m_Instance.GetHandle(), m_surface, nullptr);
			m_surface = VK_NULL_HANDLE;
		}
	}
}