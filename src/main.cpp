#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <optional>
#include <set>
#include <algorithm>

//Ready for this one:
//https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> requestedValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    /*
    VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger);
    */
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if( func != nullptr )
    {
        return func(instance, pCreateInfo, pAllocator, pMessenger);
    }
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    /*
    VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator);
    */
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
}

struct QueueFamilyIndices 
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails 
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication {
public:
    void run(const char* title) 
    {
        initWindow(WIDTH, HEIGHT, title);
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* _window = nullptr;
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue _graphicsQueue = VK_NULL_HANDLE;
    VkQueue _presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) 
    {
        std::cerr << "VK_DEBUG: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void initWindow(int width, int height, const char* title) 
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    void initVulkan() 
    {
        createInstance();
        setupDebugMessanger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
    }

    void createGraphicsPipeline() 
    {
        //auto vertShaderCode = readFile("resources/shaders/vert.spv");
        //auto fragShaderCode = readFile("resources/shaders/frag.spv");
    }

    void createSurface() 
    {
        if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) 
            throw std::runtime_error("failed to create window surface!");
    }

    void createImageViews() 
    {
        _swapChainImageViews.resize(_swapChainImages.size());

        for (auto i = 0; i < _swapChainImages.size(); i++) 
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _swapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _swapChainImageFormat;

            //allows for remapping channels
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) 
                throw std::runtime_error("failed to create image views!");
        }
    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        if( deviceCount == 0)
            throw std::runtime_error("Unable to find any devices which support Vulkan.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        for(auto device : devices)
        {
            if( isDeviceSuitable(device) )
            {
                _physicalDevice = device;
                break;
            }
        }

        if(_physicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("Unable to find a suitable device.");
    }

    void createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        float queuePriority = 1.f;
        for(auto queueFamilyIndex : uniqueQueueFamilies )
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        //default for now, to be filled in later
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(requestedValidationLayers.size());
            createInfo.ppEnabledLayerNames = requestedValidationLayers.data();
        } 
        else 
            createInfo.enabledLayerCount = 0;

        if(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) 
            throw std::runtime_error("Unable to create logical device...");

        vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
    }

    
    void createSwapChain() 
    {
        auto swapChainSupport = querySwapChainSupport(_physicalDevice);

        auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        _swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities);
        _swapChainImageFormat = surfaceFormat.format;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
            imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = _swapChainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        auto indices = findQueueFamilies(_physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } 
        else 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        if( vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS )
            throw std::runtime_error("Failed to create swap chain.");

        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures = {};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        auto indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) 
        {
            auto swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
            && deviceFeatures.geometryShader 
            && indices.isComplete()
            && extensionsSupported
            && swapChainAdequate;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
                return availableFormat;
        }

        return availableFormats[0];
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        {
            return capabilities.currentExtent;
        } 
        else 
        {
            VkExtent2D actualExtent = {WIDTH, HEIGHT};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
                return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableDeviceExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for( const auto &extension : availableDeviceExtensions )
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) 
    {
        SwapChainSupportDetails details = {};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) 
    {
        QueueFamilyIndices indices;

        uint32_t deviceQueueFamilyCount  = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &deviceQueueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &deviceQueueFamilyCount, deviceQueueFamilyProperties.data());

        int i = 0;
        for(const auto& properties : deviceQueueFamilyProperties)
        {
            if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                indices.graphicsFamily = i;

            VkBool32 supportsSurface = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &supportsSurface);
            if(properties.queueCount > 0 && supportsSurface)
                indices.presentFamily = i;

            if (indices.isComplete()) 
                break;

            i++;
        }

        return indices;
    }

    void populateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    void setupDebugMessanger()
    {
        if( !enableValidationLayers ) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        populateDebugUtilsMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    bool checkRequiredValidationLayers()
    {
        //see what layers are supported
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        //debug
        std::cout << "Available Layers:" << std::endl;
        for( auto layer : availableLayers)
            std::cout << "\t" << layer.layerName << std::endl;

        //validate them here
        for(auto requestedLayer : requestedValidationLayers)
        {
            bool foundLayer = false;
            for(auto availableLayer : availableLayers)
            {
                //is this the layer we are looking for?
                if( strcmp(requestedLayer, availableLayer.layerName) == 0)
                {
                    foundLayer = true;
                    break; //it is, great check for the next one
                }
            }

            if(!foundLayer)
                throw std::runtime_error("Missing a requested validation layer...");
        }

        //debug
        std::cout << "Continuing with validation layers:" << std::endl;
        for( auto layer : requestedValidationLayers)
            std::cout << "\t" << layer << std::endl;

        return true;
    }

    std::vector<const char*> getRequiredExtensions() 
    {
        //query the vulkan api to see what extensions are available
        uint32_t availableExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

        //debug
        std::cout << "Available Extensions:" << std::endl;
        for( auto extension : availableExtensions)
            std::cout << "\t" << extension.extensionName << std::endl;

        //get the required extensions from glfw
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensionsToLoad(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if(enableValidationLayers)
            extensionsToLoad.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        //validate the extensions here
        for(auto extensionToLoad : extensionsToLoad)
        {
            bool found = false;
            for(auto availableExtension : availableExtensions )
            {
                if(strcmp(extensionToLoad, availableExtension.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if( !found ) 
                throw std::runtime_error("Missing a required extension..."); //TODO: figure out a string concat to display which one
        }

        std::cout << "Continuing with extensions:" << std::endl;
        for( auto extension : extensionsToLoad)
            std::cout << "\t" << extension << std::endl;

        return extensionsToLoad;
    }

    void createInstance()
    {
        VkApplicationInfo applicationInfo = {}; //TODO: look this up, i am curious if it inits to empty
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Vulkan Learning Application";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
        applicationInfo.apiVersion = VK_API_VERSION_1_1;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledLayerCount = 0;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        if( enableValidationLayers && checkRequiredValidationLayers()) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(requestedValidationLayers.size());
            createInfo.ppEnabledLayerNames = requestedValidationLayers.data();

            populateDebugUtilsMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        
        if( vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS )
            throw std::runtime_error("failed to create instance!");
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() 
    {
        for (auto imageView : _swapChainImageViews) {
            vkDestroyImageView(_device, imageView, nullptr);
        }

        if ( _swapChain != VK_NULL_HANDLE )
            vkDestroySwapchainKHR(_device, _swapChain, nullptr);
        
        if( _device != VK_NULL_HANDLE )
            vkDestroyDevice(_device, nullptr);

        if( enableValidationLayers )
            DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);

         if( _surface != VK_NULL_HANDLE )
             vkDestroySurfaceKHR(_instance, _surface, nullptr);

        vkDestroyInstance(_instance, nullptr);

        if( _window != nullptr)
        {
            glfwDestroyWindow(_window);
            _window = nullptr;
        }
            
        glfwTerminate();
    }
};

int main() 
{
    HelloTriangleApplication app;

    try 
    {
        app.run("A Vulkan sample...");
    } 
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}