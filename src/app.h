#ifndef APP_H

#define APP_WIDTH   800
#define APP_HEIGHT  600

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <rphii/vec.h>

VEC_INCLUDE(VCs, vcs, const char *, BY_VAL, BASE);
VEC_INCLUDE(VCs, vcs, const char *, BY_VAL, ERR);
#define VCS(...)    (VCs){ \
        .items = (const char *[]){__VA_ARGS__},\
        .last = sizeof((const char *[]){__VA_ARGS__})/sizeof(*(const char *[]){__VA_ARGS__}), \
    }
#define RVCS(...)    (RVCs){ \
        .items = (const char *[]){__VA_ARGS__},\
        .last = sizeof((const char *[]){__VA_ARGS__})/sizeof(*(const char *[]){__VA_ARGS__}), \
    }

#include "v/VVkLayerProperties.h"
#include "v/VVkPhysicalDevice.h"
#include "v/VVkQueueFamilyProperties.h"
#include "v/VVkDeviceQueueCreateInfo.h"
#include "v/VVkExtensionProperties.h"
#include "v/VVkImage.h"
#include "v/VVkImageView.h"

#include "swap_chain_support.h"
#include "queue_family.h"
#include "log.h"

typedef struct App {
    const char *name;   // window name
    const char *engine; // engine name
    Log log;
    GLFWwindow *window;
    VCs required_extensions;
    const RVCs device_extensions;
    struct {
        const RVCs layers;
        bool enable;
        VkDebugUtilsMessengerEXT messenger;
    } validation;
    VkInstance instance;
    struct {
        QueueFamilyIndices indices;
        VkPhysicalDevice active;
        VVkPhysicalDevice available;
    } physical;
    VkDevice device;
    VkQueue graphics_queue;
    VkSurfaceKHR surface;
    VkQueue present_queue;
    VkSwapchainKHR swap_chain;
    VVkImage swap_chain_images;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    VVkImageView swap_chain_image_views;
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
} App;

int app_init(App *app);
int app_free(App *app);

#define APP_H
#endif

