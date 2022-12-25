#ifndef BACKEND_H
#define BACKEND_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace VulkanPrototype
{
    namespace Backend
    {
        int Initialize(int width, int height);
        void Cleanup();

        float GetMonitorScale();

        // TODO: Create Window Wrapper for different backends
        GLFWwindow* GetWindow();
    }
}

#endif // BACKEND_H