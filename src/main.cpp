#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

//Ready for this one:
//https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Instance

const int WIDTH = 800;
const int HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() 
    {
        initWindow(WIDTH, HEIGHT, "Vulkan");
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* _window = nullptr;

    void initWindow(int width, int height, const char* title) 
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    void initVulkan() 
    {

    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() 
    {
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
        app.run();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}