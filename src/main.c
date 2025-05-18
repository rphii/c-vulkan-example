#include "app.h"
#include "util.h"
#include "vec.h"

int main() {

    int err = 0;
    App app = {
        .name = "c-vulkan",
        .engine = "c-vulkan",
#if defined(NDEBUG)
        .validation.enable = false,
#else
        .validation.enable = true,
        .validation.layers = RVCS(
                "VK_LAYER_KHRONOS_validation",
                ),
#endif
        .device_extensions = RVCS(
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                ),
    };

    try(app_init(&app));
#if 0
    while(!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();
    }
#endif

clean:
    app_free(&app);
    return err;
error:
    err = -1;
    goto clean;

}

