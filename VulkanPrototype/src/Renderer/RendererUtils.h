#ifndef RENDERERUTILS_H
#define RENDERERUTILS_H

#include <array>
#include <fstream>
#include <optional>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace VulkanPrototype::Renderer
{
    /*
    * Helper Structs for the Renderer
    */

    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
    };

    struct FrameData
    {
        VkSemaphore     semaphoreImageAvailable;
        VkSemaphore     semaphoreRenderingDone;
        VkFence         fenceCommandBufferDone;

        VkCommandPool   commandPool;
        VkCommandBuffer mainCommandBuffer;

        AllocatedBuffer uniformBuffer;
        AllocatedBuffer objectBuffer;

        VkDescriptorSet descriptorSet;
    };

    struct GameObjectData
    {
        glm::vec3 globalPosition;
    };

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
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 textureCoordinate;

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
        static VkVertexInputBindingDescription getBindingDescription();
    };

    struct UBOValues
    {
        //Model
        float angle;
        glm::vec3 axis;

        //View
        glm::vec3 eye;
        glm::vec3 center;

        //Proj
        float fovy;
        float near;
        float far;
    };
}

#endif // RENDERERUTILS_H