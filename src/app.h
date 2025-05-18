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

#include "v/VVkLayerProperties.h"
#include "v/VVkPhysicalDevice.h"
#include "v/VVkQueueFamilyProperties.h"
#include "v/VVkDeviceQueueCreateInfo.h"

#include "queue_family.h"
#include "log.h"

typedef struct App {
    const char *name;   // window name
    const char *engine; // engine name
    Log log;
    GLFWwindow *window;
    VCs required_extensions;
    struct {
        VCs layers;
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
} App;

int app_init(App *app);
int app_free(App *app);

#define APP_H
#endif

