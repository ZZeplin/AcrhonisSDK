# CMakeList.txt : CMake project for NyxisEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Achronis "src/main.cpp" "src/Window.cpp" "src/VulkanContext.cpp" "src/VulkanContext.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Achronis PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory(deps/glfw-3.4 deps/imgui-1.92.8)

find_package(Vulkan REQUIRED)

target_include_directories(Achronis PRIVATE 

	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/deps/glfw-3.4/include

)

target_link_libraries(Achronis

	PRIVATE
	glfw
	Vulkan::Vulkan

)
