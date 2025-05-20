#ifndef APP_H

#define APP_WIDTH   800
#define APP_HEIGHT  600
#define APP_MAX_FRAMES_IN_FLIGHT    2

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <rphii/vec.h>

#if 0
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
#endif

#include "swap_chain_support.h"
#include "queue_family.h"
#include "log.h"

typedef struct App {
    const char *name;   // window name
    const char *engine; // engine name
    Log log;
    GLFWwindow *window;
    char const **required_extensions;
    char const **device_extensions;
    struct {
        char const **layers;
        bool enable;
        VkDebugUtilsMessengerEXT messenger;
    } validation;
    VkInstance instance;
    struct {
        QueueFamilyIndices indices;
        VkPhysicalDevice active;
        VkPhysicalDevice *available;
    } physical;
    VkDevice device;
    VkQueue graphics_queue;
    VkSurfaceKHR surface;
    VkQueue present_queue;
    VkSwapchainKHR swap_chain;
    VkImage *swap_chain_images;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    VkImageView *swap_chain_image_views;
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkFramebuffer *swap_chain_framebuffers;
    VkCommandPool command_pool;
    VkCommandBuffer *command_buffer;
    VkSemaphore *image_available_semaphore;
    VkSemaphore *render_finished_semaphore;
    VkFence *in_flight_scene;
    uint32_t current_frame;
    bool framebuffer_resized;
} App;

int app_init(App *app);
void app_free(App *app);
int app_render(App *app);

#define APP_H
#endif

