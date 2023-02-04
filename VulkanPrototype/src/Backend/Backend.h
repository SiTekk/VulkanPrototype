#ifndef BACKEND_H
#define BACKEND_H

#include <GLFW/glfw3.h>

namespace VulkanPrototype
{
    namespace Backend
    {
        int Initialize(int width, int height);
        void Cleanup();

        float GetMonitorScale();

        extern GLFWwindow* g_window;
    }
}

#endif // BACKEND_H