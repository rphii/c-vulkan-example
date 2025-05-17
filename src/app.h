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

VEC_INCLUDE(VVkLayerProperties, vVkLayerProperties, VkLayerProperties, BY_VAL, BASE);
VEC_INCLUDE(VVkLayerProperties, vVkLayerProperties, VkLayerProperties, BY_VAL, ERR);

typedef struct App {
    const char *name;   // window name
    const char *engine; // engine name
    GLFWwindow *window;
    VkInstance instance;
    VCs required_extensions;
    struct {
        VCs layers;
        bool enable;
    } validation;
} App;

int app_init(App *app);
int app_free(App *app);

#define APP_H
#endif

