#include "Backend.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace VulkanPrototype
{
    namespace Backend
    {
        GLFWwindow* g_window = nullptr;

        int Initialize(int width, int height)
        {
            if (!glfwInit())
            {
                std::cerr << "Could not initalize GLFW!\n";
                return -1;
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            if (!glfwVulkanSupported())
            {
                std::cerr << "GLFW: Vulkan not supported!\n";
                return -1;
            }

            g_window = glfwCreateWindow(width, height, "VulkanPrototype", nullptr, nullptr);

            return 0;
        }

        void Cleanup()
        {
            glfwDestroyWindow(g_window);
        }

        float GetMonitorScale()
        {
            GLFWmonitor *primary = glfwGetPrimaryMonitor();
            float xscale, yscale;
            glfwGetMonitorContentScale(primary, &xscale, &yscale);

            return xscale > yscale ? xscale : yscale;
        }
    }
}
