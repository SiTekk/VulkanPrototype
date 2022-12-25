#include "Backend.h"

namespace VulkanPrototype
{
    static GLFWwindow* window;

    int Backend::Initialize(int width, int height)
    {
        if (!glfwInit())
        {
            std::cerr << "Could not initalize GLFW!\n";
            return -1;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        if (!glfwVulkanSupported())
        {
            std::cerr << "GLFW: Vulkan not supported!\n";
            return -1;
        }

        window = glfwCreateWindow(width, height, "VulkanPrototype", nullptr, nullptr);

        return 0;
    }

    void Backend::Cleanup()
    {
        glfwDestroyWindow(window);
    }

    float Backend::GetMonitorScale()
    {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        float xscale, yscale;
        glfwGetMonitorContentScale(primary, &xscale, &yscale);

        return xscale > yscale ? xscale : yscale;
    }

    GLFWwindow* Backend::GetWindow()
    {
        return window;
    }
}
