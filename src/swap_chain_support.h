#ifndef SWAP_CHAIN_SUPPORT_DETAILS

#include <vulkan/vulkan.h>
#include "vec.h"

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats;
    VkPresentModeKHR *present_modes;
} SwapChainSupportDetails;

int swap_chain_support_query(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails *details);
void swap_chain_support_free(SwapChainSupportDetails *details);

#define SWAP_CHAIN_SUPPORT_DETAILS
#endif

