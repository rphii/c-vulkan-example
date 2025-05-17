#include "app.h"
#include "util.h"

VEC_IMPLEMENT(VCs, vcs, const char *, BY_VAL, BASE, 0);
VEC_IMPLEMENT(VCs, vcs, const char *, BY_VAL, ERR);

VEC_IMPLEMENT(VVkLayerProperties, vVkLayerProperties, VkLayerProperties, BY_VAL, BASE, 0);
VEC_IMPLEMENT(VVkLayerProperties, vVkLayerProperties, VkLayerProperties, BY_VAL, ERR);

#include <cglm/cglm.h>

int app_init_window(App *app) { /*{{{*/
    assert_arg(app);
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    app->window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, app->name, 0, 0);
    println(">>> glfw initialized");
    return (!app->window);
} /*}}}*/

bool app_init_check_validation_support(App *app) {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, 0);
    VVkLayerProperties available_layers = {0};
    try(vVkLayerProperties_resize(&available_layers, layer_count));
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.items);
    for(size_t j = 0; j < vcs_length(app->validation.layers); ++j) {
        const char *layer_name = vcs_get_at(&app->validation.layers, j);
        bool layer_found = false;
        for(size_t i = 0; i < vVkLayerProperties_length(available_layers); ++i) {
            VkLayerProperties layer_props = vVkLayerProperties_get_at(&available_layers, i);
            if(strcmp(layer_name, layer_props.layerName)) continue;
            layer_found = true;
            break;
        }
        if(!layer_found) return false;
        println(">>> found %s", layer_name);
    }
    vVkLayerProperties_free(&available_layers);
    return true;
error:
    vVkLayerProperties_free(&available_layers);
    return false;
}

int get_required_extensions(App *app, VCs *required_extensions) {
    vcs_clear(required_extensions);
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    for(size_t i = 0; i < glfw_extension_count; ++i) {
        try(vcs_push_back(required_extensions, glfw_extensions[i]));
    }
    if(app->validation.enable) {
        try(vcs_push_back(required_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    }
    // macOS
    try(vcs_push_back(required_extensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME));
    return 0;
error:
    return -1;
}


int app_init_vulkan_create_instance(App *app) { /*{{{*/
    assert_arg(app);
    println(">>> initialize vulkan > create instance");

    println(">>> initialize vulkan > check validation support");
    if(app->validation.enable && !app_init_check_validation_support(app)) {
        THROW("validation layers requested, but not available!");
    }
    try(get_required_extensions(app, &app->required_extensions));

    println(">>> initialize vulkan > set app info");
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app->name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = app->engine;
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    println(">>> initialize vulkan > set create info");
    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = vcs_length(app->required_extensions);
    create_info.ppEnabledExtensionNames = vcs_iter_begin(app->required_extensions);
    if(app->validation.enable) {
        println(">>> enable %zu validation layers", vcs_length(app->validation.layers));
        println(">>> first one %s", vcs_get_front(&app->validation.layers));
        create_info.enabledLayerCount = vcs_length(app->validation.layers);
        create_info.ppEnabledLayerNames = vcs_iter_begin(app->validation.layers);
    } else {
        create_info.enabledLayerCount = 0;
    }
    try(vkCreateInstance(&create_info, 0, &app->instance));

    println(">>> initialize vulkan > created instance");
    return 0;
error:
    return -1;
} /*}}}*/

int app_init_vulkan(App *app) { /*{{{*/
    assert_arg(app);
    println(">>> initialize vulkan");
    try(app_init_vulkan_create_instance(app));
    return 0;
error:
    return -1;
} /*}}}*/

int app_init(App *app) { /*{{{*/
    assert_arg(app);
    assert_arg(app->name);
    assert_arg(app->engine);
    try(app_init_window(app));
    try(app_init_vulkan(app));
    return 0;
error:
    return -1;
} /*}}}*/

int app_free(App *app) { /*{{{*/
    assert_arg(app);
    vkDestroyInstance(app->instance, 0);
    vcs_free(&app->required_extensions);
    glfwDestroyWindow(app->window);
    glfwTerminate();
    return 0;
} /*}}}*/


