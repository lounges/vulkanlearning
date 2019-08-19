#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

//Ready for this one:
//https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> requestedValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
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
    VkDebugUtilsMessengerEXT _debugMessenger;
    VkInstance _instance;

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
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) 
    {

        std::cerr << "VK_DEBUG: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void setupDebugMessanger()
    {
        if( !enableValidationLayers ) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

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

        if( enableValidationLayers && checkRequiredValidationLayers()) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(requestedValidationLayers.size());
            createInfo.ppEnabledLayerNames = requestedValidationLayers.data();
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
        if( enableValidationLayers )
            DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);

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