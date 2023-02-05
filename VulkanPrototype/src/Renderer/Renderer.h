#ifndef RENDERER_H
#define RENDERER_H

#define VULKANPROTOTYPE_VERSION VK_MAKE_VERSION(0, 1, 0)

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

#include "RendererUtils.h"
#include "../Backend/Backend.h"

namespace VulkanPrototype::Renderer
{
    /*
     * Global Variables
     */
    extern VkExtent2D g_windowSize;
    extern UBOValues g_uboValues;

    /*
     * Global Functions
     */

    void Cleanup();
    int  Initialize();
    void RenderFrame(ImDrawData* draw_data);

    /*
     * Private Functions
     */

     //Begin and End of Command Buffers
    void beginCommandBuffer(VkCommandBuffer* commandBuffer);
    void endCommandBuffer(VkCommandBuffer commandBuffer);

    bool checkInstanceExtensionSupport(std::vector<const char*> instanceExtensions);
    bool checkInstanceLayerSupport(std::vector<const char*> instanceLayers);

    VkExtent2D chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void copyBuffer(uint64_t size, VkBuffer srcBuffer, VkBuffer dstBuffer);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void createBuffer(uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void createCommandBuffers();
    void createCommandPool();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets();
    void createFramebuffers();
    void createGraphicsPipeline();
    void createImage(const VkImageCreateInfo& imageCreateInfo, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(const VkImage image, const VkFormat format);
    void createImageViews();
    void createIndexBuffer();
    int createInstance();
    void createLogicalDevice(VkPhysicalDevice physicalDevice);
    void createRenderPass();
    void createSemaphores();
    void createShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);
    void createSwapchain(VkPhysicalDevice physicalDevice);
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void createUniformBuffers();
    void createVertexBuffer();

    uint32_t pickMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkPhysicalDevice pickPhysicalDevice();
    QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice);

    //TODO: Maybe pass SurfaceDetails as reference: void querySurfaceCapabilities(VkPhysicalDevice physicalDevice, SurfaceDetails& aceDetails)
    SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice);

    void readFile(const std::string& filename, std::vector<char>& buffer);

    void recreateGraphicsPipelineAndSwapchain();

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void updateUniformBuffer(uint32_t imageIndex);
}

#endif // RENDERER_H