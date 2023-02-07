#ifndef RENDERER_H
#define RENDERER_H

#define VULKANPROTOTYPE_VERSION VK_MAKE_VERSION(0, 1, 0)

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "RendererUtils.h"

namespace VulkanPrototype::Renderer
{
    /*
     * Global Functions
     */

    void Cleanup();
    int  Initialize();
    void RenderFrame(ImDrawData* draw_data);

    /*
     * Global Variables
     */

    extern VkExtent2D g_windowSize;
    extern UBOValues g_uboValues;
}

#endif // RENDERER_H