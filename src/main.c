#include "app.h"
#include "util.h"
#include "vec.h"

int main() {

    int err = 0;
    App app = {
        .name = "c-vulkan",
        .engine = "c-vulkan",
    };
#if !defined(NDEBUG)
    app.validation.enable = true;
#endif

    try(app_init(&app));
#if 1
    double t0 = glfwGetTime();
    size_t frames = 0;
    while(!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();
        try(app_render(&app));
        ++frames;
        double tX = glfwGetTime();
        if(tX - t0 > 2.0) {
            printf("%9.1f fps\n", (double)frames/(tX-t0));
            frames = 0;
            t0 = tX;
        }
    }
#endif

clean:
    app_free(&app);
    return err;
error:
    err = -1;
    goto clean;

}

