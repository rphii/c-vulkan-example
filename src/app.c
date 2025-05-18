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
    for(size_t j = 0; j < rvcs_length(app->validation.layers); ++j) {
        const char *layer_name = rvcs_get_at(&app->validation.layers, j);
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
        log_info(&app->log, "enable %zu validation layers", rvcs_length(app->validation.layers));
        create_info.enabledLayerCount = rvcs_length(app->validation.layers);
        create_info.ppEnabledLayerNames = rvcs_iter_begin(app->validation.layers);
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

int app_init_vulkan_create_surface(App *app) {
    assert_arg(app);
    log_down(&app->log, "create surface");
    try(glfwCreateWindowSurface(app->instance, app->window, 0, &app->surface));
    log_ok(&app->log, "created surface");
    log_up(&app->log);
    return 0;
error:
    log_up(&app->log);
    return -1;
}

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

int find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilyIndices *indices) { /*{{{*/
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
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if(present_support) {
            optional_u32_set(&indices->present_family, i);
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

bool check_device_extension_support(VkPhysicalDevice device, RVCs device_extensions) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, 0, &extension_count, 0);
    VVkExtensionProperties extension_properties = {0};
    bool found = false;
    try(vVkExtensionProperties_resize(&extension_properties, extension_count));
    vkEnumerateDeviceExtensionProperties(device, 0, &extension_count, vVkExtensionProperties_iter_begin(extension_properties));
    for(size_t j = 0; j < rvcs_length(device_extensions); ++j) {
        const char *required = rvcs_get_at(&device_extensions, j);
        found = false;
        for(size_t i = 0; i < vVkExtensionProperties_length(extension_properties); ++i) {
            VkExtensionProperties extension = vVkExtensionProperties_get_at(&extension_properties, i);
            if(strcmp(extension.extensionName, required)) continue;
            found = true;
            break;
        }
        if(!found) goto clean;
    }
clean:
    vVkExtensionProperties_free(&extension_properties);
    return found;
error:
    found = false;
    goto clean;
}

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilyIndices *indices, RVCs device_extensions) { /*{{{*/
    bool extensions_supported = false;
    try(find_queue_families(device, surface, indices));
    extensions_supported = check_device_extension_support(device, device_extensions);
clean:
    return queue_family_indices_is_complete(indices) && extensions_supported;
error:
    goto clean;
} /*}}}*/

int app_init_vulkan_pick_physical_device(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "pick physical device");
    try(app_init_vulkan_refresh_physical_devices(app));
    for(size_t i = 0; i < vVkPhysicalDevice_length(app->physical.available); ++i) {
        VkPhysicalDevice device = vVkPhysicalDevice_get_at(&app->physical.available, i);
        if(is_device_suitable(device, app->surface, &app->physical.indices, app->device_extensions)) {
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
    int err = 0;

    VVkDeviceQueueCreateInfo queue_create_infos = {0};
    uint32_t queue_families[] = {
        app->physical.indices.graphics_family.value,
        app->physical.indices.present_family.value,
    };
    float queue_priority = 1.0f;
    for(size_t i = 0; i < sizearray(queue_families); ++i) {
        uint32_t queue_family = queue_families[i];
        bool skip = false;
        for(size_t j = 0; j < i; ++j) {
            if(queue_families[j] == queue_family) {
                skip = true;
                break;
            }
        }
        if(skip) continue;
        /* unique queue family found */
        VkDeviceQueueCreateInfo queue_create_info = {0};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        try(vVkDeviceQueueCreateInfo_push_back(&queue_create_infos, queue_create_info));
    }

    VkPhysicalDeviceFeatures device_features = {0};
    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = vVkDeviceQueueCreateInfo_iter_begin(queue_create_infos);
    create_info.queueCreateInfoCount = vVkDeviceQueueCreateInfo_length(queue_create_infos);
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = 0;
    if(app->validation.enable) {
        create_info.enabledLayerCount = rvcs_length(app->validation.layers);
        create_info.ppEnabledLayerNames = rvcs_iter_begin(app->validation.layers);
    } else {
        create_info.enabledLayerCount = 0;
    }
    try(vkCreateDevice(app->physical.active, &create_info, 0, &app->device));

    log_info(&app->log, "get graphics queue");
    vkGetDeviceQueue(app->device, app->physical.indices.graphics_family.value, 0, &app->graphics_queue);
    log_info(&app->log, "get present queue");
    vkGetDeviceQueue(app->device, app->physical.indices.present_family.value, 0, &app->present_queue);
    log_ok(&app->log, "created logical device");
    log_up(&app->log);
clean:
    vVkDeviceQueueCreateInfo_free(&queue_create_infos);
    return err;
error:
    log_up(&app->log);
    err = -1;
    goto clean;
} /*}}}*/

int app_init_vulkan(App *app) { /*{{{*/
    assert_arg(app);
    log_down(&app->log, "initialize vulkan");
    try(app_init_vulkan_create_instance(app));
    try(app_init_vulkan_setup_debug_messenger(app));
    try(app_init_vulkan_create_surface(app));
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
    if(app->surface) {
        log_info(&app->log, "destroy surface");
        vkDestroySurfaceKHR(app->instance, app->surface, 0);
    }
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


