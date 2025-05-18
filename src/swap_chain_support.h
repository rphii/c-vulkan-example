#ifndef SWAP_CHAIN_SUPPORT_DETAILS

#include "v/VVkSurfaceFormatKHR.h"
#include "v/VVkPresentModeKHR.h"

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VVkSurfaceFormatKHR formats;
    VVkPresentModeKHR present_modes;
} SwapChainSupportDetails;

int swap_chain_support_query(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails *details);
void swap_chain_support_free(SwapChainSupportDetails *details);

#define SWAP_CHAIN_SUPPORT_DETAILS
#endif

