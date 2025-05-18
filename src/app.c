#include "app.h"
#include "util.h"
#include <rphii/colorprint.h>

VEC_IMPLEMENT(VCs, vcs, const char *, BY_VAL, BASE, 0);
VEC_IMPLEMENT(VCs, vcs, const char *, BY_VAL, ERR);

#include <cglm/cglm.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback( /*{{{*/
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
    println("validation layer: %s", pCallbackData->pMessage);
    return VK_FALSE;
} /*}}}*/

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info) { /*{{{*/
    assert_arg(create_info);
    memset(create_info, 0, sizeof(*create_info));
    create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info->pfnUserCallback = debug_callback;
    create_info->pUserData = 0; // optional
} /*}}}*/

int app_init_window(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "initialize glfw");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    app->window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, app->name, 0, 0);
    log_ok(&app->log, "initialized glfw");
    log_up(&app->log);
    return (!app->window);
} /*}}}*/

bool app_init_check_validation_support(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "check validation layer support");
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
        log_info(&app->log, "found %s", layer_name);
    }
    vVkLayerProperties_free(&available_layers);
    log_ok(&app->log, "validataion layer supported");
    log_up(&app->log);
    return true;
error:
    vVkLayerProperties_free(&available_layers);
    log_up(&app->log);
    return false;
} /*}}}*/

int get_required_extensions(App *app, VCs *required_extensions) { /*{{{*/
    assert_arg(app);
    assert_arg(required_extensions);
    log_down(&app->log, "get required extensions");
    vcs_clear(required_extensions);
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    for(size_t i = 0; i < glfw_extension_count; ++i) {
        log_info(&app->log, "require %s", glfw_extensions[i]);
        try(vcs_push_back(required_extensions, glfw_extensions[i]));
    }
    if(app->validation.enable) {
        log_info(&app->log, "require %s", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        try(vcs_push_back(required_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    }
    // macOS
    log_info(&app->log, "require %s", VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    try(vcs_push_back(required_extensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME));
    log_ok(&app->log, "got required extensions");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int app_init_vulkan_create_instance(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "create instance");

    if(app->validation.enable && !app_init_check_validation_support(app)) {
        THROW("validation layers requested, but not available!");
    }
    try(get_required_extensions(app, &app->required_extensions));

    log_info(&app->log, "set app info");
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app->name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = app->engine;
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    log_info(&app->log, "set create info");
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = vcs_length(app->required_extensions);
    create_info.ppEnabledExtensionNames = vcs_iter_begin(app->required_extensions);
    if(app->validation.enable) {
        populate_debug_messenger_create_info(&debug_create_info);
        log_info(&app->log, "enable %zu validation layers", vcs_length(app->validation.layers));
        create_info.enabledLayerCount = vcs_length(app->validation.layers);
        create_info.ppEnabledLayerNames = vcs_iter_begin(app->validation.layers);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.pNext = 0;
    }
    try(vkCreateInstance(&create_info, 0, &app->instance));

    log_ok(&app->log, "created instance");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) { /*{{{*/
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(!func) THROW("could not find function");
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
error:
    return -1;
} /*}}}*/

VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) { /*{{{*/
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT )
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(!func) THROW("could not find function");
    func(instance, debugMessenger, pAllocator);
    return 0;
error:
    return -1;
} /*}}}*/

int app_init_vulkan_setup_debug_messenger(App *app) { /*{{{*/
    assert_arg(app);
    if(!app->validation.enable) return 0;
    log_down(&app->log, "set up debug messenger");
    VkDebugUtilsMessengerCreateInfoEXT create_info = {0};
    populate_debug_messenger_create_info(&create_info);
    /* call extension function */
    try(CreateDebugUtilsMessengerEXT(app->instance, &create_info, 0, &app->validation.messenger));
    log_ok(&app->log, "set up debug messenger");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int app_init_vulkan_refresh_physical_devices(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "refresh available physical devices");

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(app->instance, &device_count, 0);
    if(!device_count) {
        THROW("failed to find GPUs with vulkan support!");
    }
    try(vVkPhysicalDevice_resize(&app->physical.available, device_count));
    vkEnumeratePhysicalDevices(app->instance, &device_count, vVkPhysicalDevice_iter_begin(app->physical.available));
    for(size_t i = 0; i < vVkPhysicalDevice_length(app->physical.available); ++i) {
        VkPhysicalDevice device = vVkPhysicalDevice_get_at(&app->physical.available, i);
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(device, &device_features);
        log_info(&app->log, "found '%s'", device_properties.deviceName);
    }

    log_ok(&app->log, "refreshed available physical devices");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int find_queue_families(VkPhysicalDevice device, QueueFamilyIndices *indices) { /*{{{*/
    assert_arg(indices);
    int err = 0;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VVkQueueFamilyProperties queue_families = {0};
    try(vVkQueueFamilyProperties_resize(&queue_families, queue_family_count));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.items);
    for(size_t i = 0; i < vVkQueueFamilyProperties_length(queue_families); ++i) {
        VkQueueFamilyProperties queue_family = vVkQueueFamilyProperties_get_at(&queue_families, i);
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            optional_u32_set(&indices->graphics_family, i);
        }
    }
clean:
    vVkQueueFamilyProperties_free(&queue_families);
    return err;
error:
    queue_family_indices_clear(indices);
    err = -1;
    goto clean;
} /*}}}*/

bool is_device_suitable(VkPhysicalDevice device, QueueFamilyIndices *indices) { /*{{{*/
    try(find_queue_families(device, indices));
clean:
    return queue_family_indices_is_complete(indices);
error:
    goto clean;
} /*}}}*/

int app_init_vulkan_pick_physical_device(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "pick physical device");
    try(app_init_vulkan_refresh_physical_devices(app));
    for(size_t i = 0; i < vVkPhysicalDevice_length(app->physical.available); ++i) {
        VkPhysicalDevice device = vVkPhysicalDevice_get_at(&app->physical.available, i);
        if(is_device_suitable(device, &app->physical.indices)) {
            app->physical.active = device; // TODO actually pick a suitable physical device
            log_info(&app->log, "found suitable device");
            break;
        }
    }
    if(!app->physical.active) {
        THROW("no suitable device found");
    }
    log_ok(&app->log, "picked physical device");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int app_init_vulkan_create_logical_device(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "create logical device");
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {0};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = app->physical.indices.graphics_family.value;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    VkPhysicalDeviceFeatures device_features = {0};
    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = 0;
    if(app->validation.enable) {
        create_info.enabledLayerCount = vcs_length(app->validation.layers);
        create_info.ppEnabledLayerNames = vcs_iter_begin(app->validation.layers);
    } else {
        create_info.enabledLayerCount = 0;
    }
    try(vkCreateDevice(app->physical.active, &create_info, 0, &app->device));
    log_ok(&app->log, "created logical device");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int app_init_vulkan(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "initialize vulkan");
    try(app_init_vulkan_create_instance(app));
    try(app_init_vulkan_setup_debug_messenger(app));
    try(app_init_vulkan_pick_physical_device(app));
    try(app_init_vulkan_create_logical_device(app));
    log_ok(&app->log, "initialized vulkan");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
} /*}}}*/

int app_init(App *app) { /*{{{*/
    assert_arg(app);
    assert_arg(app->name);
    assert_arg(app->engine);
    log_start(&app->log);
    try(app_init_window(app));
    try(app_init_vulkan(app));
    return 0;
error:
    return -1;
} /*}}}*/

int app_free(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "clean up");
    if(app->device) {
        log_info(&app->log, "destroy logical device");
        vkDestroyDevice(app->device, 0);
    }
    if(app->validation.enable) {
        log_info(&app->log, "destroy debug messenger");
        DestroyDebugUtilsMessengerEXT(app->instance, app->validation.messenger, 0);
    }
    if(app->instance) {
        log_info(&app->log, "destroy instance");
        vkDestroyInstance(app->instance, 0);
    }
    glfwDestroyWindow(app->window);
    glfwTerminate();
    vVkPhysicalDevice_free(&app->physical.available);
    vcs_free(&app->required_extensions);
    log_ok(&app->log, "cleaned up");
    log_up(&app->log);
    return 0;
} /*}}}*/


