#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#define VULKANPROTOTYPE_VERSION VK_MAKE_VERSION(0, 1, 0)

namespace VulkanPrototype
{
    struct QueueFamily
    {
        std::optional<uint32_t> index;
        uint32_t queueCount;
    };

    struct SurfaceDetails
    {
        VkSurfaceCapabilitiesKHR capabilities = {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
        static VkVertexInputBindingDescription getBindingDescription();
    };

    struct UBOValues
    {
        //Model
        float angle;
        int axis[3];

        //View
        int eye[3];
        int center[3];
        int up[3];

        //Proj
        float fovy;
        float near;
        float far;
    };


    /// <summary>
    /// Hauptmethode, die zu Beginn des Programms aufgerufen werden muss. Hier werden alle Funktionen für die Initialisierung gerufen.
    /// </summary>
    /// <returns>Gibt nach erfolgreichem beenden 0 zurück.</returns>
    int Run();

}

#endif
