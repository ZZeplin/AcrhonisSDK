#include <iostream>
#include <stdexcept>
#include <src/vkb/vk_surface.h>

namespace vkb {

	bool CreateSurface(SurfaceRuntime& surfRuntime, InstanceRuntime& instRuntime, GLFWwindow* window) {
		if (glfwCreateWindowSurface(instRuntime.vk_instance, window, nullptr, &surfRuntime.vk_surface)) {
			return true;
		}

		return false;
	}

	void DestroySurface(SurfaceRuntime& surfRuntime, InstanceRuntime& instRuntime) {
		if (surfRuntime.vk_surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(instRuntime.vk_instance, surfRuntime.vk_surface, nullptr);
			surfRuntime.vk_surface = VK_NULL_HANDLE;
		}
	}
}