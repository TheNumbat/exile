
#include "vulkan.h"
#include <SDL2/SDL_vulkan.h>

#define VK_CHECK(f) do {                \
	VkResult res = (f);                 \
	if (res != VK_SUCCESS) {	        \
		die("VK_CHECK: %", vk_err_str(res)); \
	}} while(0);                        \

static literal vk_err_str(VkResult errorCode) {
	switch (errorCode) {
#define STR(r) case VK_##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

static void* vk_alloc(void*, usize sz, usize, VkSystemAllocationScope) {
    return Vulkan::vk_alloc_t::alloc<u8>(sz);
}

static void* vk_realloc(void*, void* mem, usize sz, usize, VkSystemAllocationScope) {
	Vulkan::vk_alloc_t::dealloc(mem);
	return Vulkan::vk_alloc_t::alloc<u8>(sz);
}

static void vk_free(void*, void* mem) {
    Vulkan::vk_alloc_t::dealloc(mem);
}

void Vulkan::init(SDL_Window* window) {
    
    assert(window);

    create_instance(window);
	enumerate_physical_devices();
	select_physical_device();
	create_logical_device_and_queues();
	create_semaphores();
	create_commandPool();
	create_commandBuffer();
	// vulkanAllocator.Init();
	// stagingManager.Init();
	create_swap_chain();
	create_render_targets();
	create_render_pass();
	create_pipeline_cache();
	create_frame_buffers();
	// renderProgManager.Init();
}

void Vulkan::destroy() {
	inst_ext.destroy();
	dev_ext.destroy();
	layers.destroy();
	phys_devices.destroy();
}

void Vulkan::create_instance(SDL_Window* window) {

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Exile";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "Exile 0.2";
    app_info.engineVersion = 2;
    app_info.apiVersion = VK_MAKE_VERSION(1, 1, VK_HEADER_VERSION);

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
	
	inst_ext.clear(); dev_ext.clear(); layers.clear();
	
	inst_ext.push(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	dev_ext.push(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	layers.push("VK_LAYER_LUNARG_standard_validation");

	u32 sdl_count;
	if(!SDL_Vulkan_GetInstanceExtensions(window, &sdl_count, null)) {
		die("Failed to get required SDL vk instance extensions: %", SDL_GetError());
	}
	inst_ext.extend(inst_ext.size + sdl_count);
	if(!SDL_Vulkan_GetInstanceExtensions(window, &sdl_count, inst_ext.data + inst_ext.size - sdl_count)) {
		die("Failed to get required SDL vk instance extensions: %", SDL_GetError());
	}

	create_info.enabledExtensionCount = inst_ext.size;
	create_info.ppEnabledExtensionNames = inst_ext.data;
	create_info.enabledLayerCount = layers.size;
	create_info.ppEnabledLayerNames = layers.data;

	VkAllocationCallbacks allocator = {};
	allocator.pfnAllocation = vk_alloc;
	allocator.pfnFree = vk_free;
	allocator.pfnReallocation = vk_realloc;

	VkResult res = vkCreateInstance(&create_info, &allocator, &instance);
	if(res != VK_SUCCESS) {
		die("Failed to create VkInstance: %", vk_err_str(res));
	}

	if(!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
		die("Failed to create SDL VkSurface: %", SDL_GetError());
	}

	info("Created Vulkan instance and surface.");
}

void Vulkan::enumerate_physical_devices() {

	u32 devices = 0;
	VkResult res = vkEnumeratePhysicalDevices(instance, &devices, null);
	if(res != VK_SUCCESS) {
		die("Failed to enumerate physical devices: %", vk_err_str(res));
	}
	if(devices <= 0) {
		die("Found no physical devices.");
	}

	vec<VkPhysicalDevice, vk_alloc_t> phys_list;
	defer(phys_list.destroy());
	phys_list.extend(devices);

	res = vkEnumeratePhysicalDevices(instance, &devices, phys_list.data);
	if(res != VK_SUCCESS) {
		die("Failed to enumerate physical devices: %", vk_err_str(res));
	}
	if(devices <= 0) {
		die("Found no physical devices.");
	}

	phys_devices.clear();
	phys_devices.extend(devices);
	for(u32 i = 0; i < devices; i++) {
		phys_device_info& info = phys_devices[i];
		info.device = phys_list[i];
	
		{
			// vkGetPhysicalDeviceQueueFamilyProperties()
		}
		{
			// vkEnumerateDeviceExtensionProperties()
		}
		{
			// vkGetPhysicalDeviceSurfaceCapabilitiesKHR()
		}
		{
			// vkGetPhysicalDeviceSurfaceFormatsKHR()
		}
		{
			// vkGetPhysicalDeviceSurfacePresentModesKHR()
		}
		{
			// vkGetPhysicalDeviceMemoryProperties()
		}
		{
			// vkGetPhysicalDeviceProperties()
		}
	}
}

void Vulkan::select_physical_device() {

}

void Vulkan::create_logical_device_and_queues() {

}

void Vulkan::create_semaphores() {

}

void Vulkan::create_commandPool() {

}

void Vulkan::create_commandBuffer() {

}

void Vulkan::create_swap_chain() {

}

void Vulkan::create_render_targets() {

}

void Vulkan::create_render_pass() {

}

void Vulkan::create_pipeline_cache() {

}

void Vulkan::create_frame_buffers() {

}
