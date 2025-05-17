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
        .validation.layers = VCS(
            "VK_LAYER_KHRONOS_validation",
            ),
#endif
    };

    try(app_init(&app));
    // while(!glfwWindowShouldClose(app.window)) {
    //     glfwPollEvents();
    // }
clean:
    app_free(&app);
    return err;
error:
    err = -1;
    goto clean;

}

