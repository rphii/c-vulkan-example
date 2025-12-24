#include <string.h>
#include "swap_chain_support.h"
#include "util.h"

void swap_chain_support_query(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails *details) {
    assert_arg(device);
    assert_arg(surface);
    memset(details, 0, sizeof(*details));
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details->capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, 0);
    if(format_count) {
        array_resize(details->formats, format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details->formats);
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, 0);
    if(present_mode_count) {
        array_resize(details->present_modes, present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details->present_modes);
    }
}

void swap_chain_support_free(SwapChainSupportDetails *details) {
    assert_arg(details);
    array_free(details->present_modes);
    array_free(details->formats);
    memset(details, 0, sizeof(*details));
}

