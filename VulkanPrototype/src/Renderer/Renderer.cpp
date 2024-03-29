﻿#include "Renderer.h"

#include<vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Backend/Backend.h"

namespace VulkanPrototype::Renderer
{
    /*
    * Global Variables
    */

    UBOValues g_uboValues =
    {
        .angle = 0.f,
        .axis = {0.f, 0.f, 1.f},
        .eye = {0.f, 0.f, 0.f},
        .center = {0.f, 0.f, 1.f},
        .up = { 0.0f, -1.0f, 0.0f },
        .fovy = 60.f,
        .near = 0.1f,
        .far = 1000.f
    };

    VkExtent2D g_windowSize = { 1600, 900 };

    VkPolygonMode g_polygonMode = VK_POLYGON_MODE_FILL;

    /*
    * Module Global Variables
    */

    static uint32_t imageCount = 0;

    //Set to custom allocator if needed
    static VkAllocationCallbacks* pAllocator = nullptr;

    static std::vector<FrameData> frames;

    //Buffers
    //static VkBuffer indexBuffer;
    //static VkDeviceMemory indexBufferMemory;
    static AllocatedBuffer indexBuffer;
    //static VkBuffer vertexBuffer;
    //static VkDeviceMemory vertexBufferMemory;
    static AllocatedBuffer vertexBuffer;
    //static std::vector<VkBuffer> uniformBuffers;
    //static std::vector<VkDeviceMemory> uniformBuffersMemory;

    static VkImage textureImage;
    static VkImageView textureImageView;
    static VkDeviceMemory textureImageMemory;
    static VkSampler textureSampler;

    static VkImage depthImage;
    static VkImageView depthImageView;
    static VkDeviceMemory depthImageMemory;

    static std::vector<VkFramebuffer> framebuffers;
    static std::vector<VkImageView> imageViews;

    //Descriptors
    static VkDescriptorPool descriptorPool;
    static VkDescriptorPool descriptorPoolImGui;
    static VkDescriptorSetLayout descriptorSetLayout;
    // static std::vector<VkDescriptorSet> descriptorSets;

    //Platformspecific
    static VkSwapchainKHR swapchain;
    static VkSurfaceKHR surface;
    static VkSurfaceFormatKHR surfaceFormat;

    //Core
    static VkDevice device;
    static VkInstance instance;
    static VkRenderPass renderPass;
    static VkPhysicalDevice physicalDevice;
    static VkPipeline pipeline;
    static VkPipeline wireframePipeline;
    static VkPipelineLayout pipelineLayout;

    static VkQueue queue;

    static QueueFamily queueFamily;

    //Vertex Buffer
    static const std::vector<Vertex> vertices =
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, .0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, .0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, .0f, .0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, 1.0f}, {.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, .0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 1.0f}, {.0f, 1.0f, .0f}, {1.0f, 1.0f}}
    };

    static const std::vector<uint16_t> indices =
    {
        0, 2, 1, 0, 3, 2,
        0, 7, 3, 0, 4, 7,
        1, 4, 0, 1, 5, 4,
        2, 5, 1, 2, 6, 5,
        3, 6, 2, 3, 7, 6,
        4, 6, 7, 4, 5, 6
    };

    glm::vec3 cubePositions[] =
    {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, 15.0f),
        glm::vec3(-1.5f, -2.2f, 2.5f),
        glm::vec3(-3.8f, -2.0f, 12.3f),
        glm::vec3(2.4f, -0.4f, 3.5f),
        glm::vec3(-1.7f, 3.0f, 7.5f),
        glm::vec3(1.3f, -2.0f, 2.5f),
        glm::vec3(1.5f, 2.0f, 2.5f),
        glm::vec3(1.5f, 0.2f, 1.5f),
        glm::vec3(-1.3f, 1.0f, 1.5f)
    };

    /*
     * Forward Declarations
     */

    void createImage(const VkImageCreateInfo& imageCreateInfo, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags);
    void createShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);
    uint32_t pickMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkPhysicalDevice pickPhysicalDevice();
    QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice);
    SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice);

    /*
     * Debug Utils
     */

#ifdef DEBUG
    static VkDebugUtilsMessengerEXT debugMessenger = nullptr;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        // Unused parameter
        (void)pUserData;
        (void)messageType;
        (void)messageSeverity;

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
#endif

    /*
     * Private Utility Functions
     */

    static void evaluteVulkanResult(VkResult result)
    {
        //TODO: Implement correct Program abortion
        if (result != VK_SUCCESS)
        {
            std::cout << result << "\n";
        }
    }

    void beginCommandBuffer(VkCommandBuffer* commandBuffer)
    {
        VkResult result;

        VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = frames[0].commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffer);
        evaluteVulkanResult(result);

        VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr
        };

        result = vkBeginCommandBuffer(*commandBuffer, &commandBufferBeginInfo);
        evaluteVulkanResult(result);
    }

    void endCommandBuffer(VkCommandBuffer commandBuffer)
    {
        VkResult result;

        result = vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };

        result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        evaluteVulkanResult(result);
        result = vkQueueWaitIdle(queue);
        evaluteVulkanResult(result);

        vkFreeCommandBuffers(device, frames[0].commandPool, 1, &commandBuffer);
    }

    void readFile(const std::string& filename, std::vector<char>& buffer)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (file.is_open())
        {
            size_t fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0);
            buffer.resize(fileSize);
            file.read(buffer.data(), fileSize);
            file.close();
        }
        else
        {
            throw std::runtime_error("Datei \"" + filename + "\" konnte nicht geoeffnet werden!");
        }
    }

    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer;
        beginCommandBuffer(&commandBuffer);

        VkImageMemoryBarrier imageMemoryBarrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {   //TODO: uncaracteristic throw
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageMemoryBarrier
        );

        endCommandBuffer(commandBuffer);
    }

    /*
     * Private Functions
     */

    bool checkInstanceExtensionSupport(std::vector<const char*> instanceExtensions)
    {
        uint32_t amountOfExtensions = 0;
        VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
        evaluteVulkanResult(result);

        std::vector<VkExtensionProperties> extensionProperties;
        extensionProperties.resize(amountOfExtensions);
        vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensionProperties.data());

        uint32_t count = 0;

        for (uint32_t i = 0; i < instanceExtensions.size(); i++)
        {
            for (uint32_t k = 0; k < amountOfExtensions; k++)
            {
                if (strcmp(instanceExtensions[i], extensionProperties[k].extensionName) == 0)
                {
                    count++;
                    break;
                }
            }
        }

        if (count != instanceExtensions.size())
            return false;

        return true;
    }

    bool checkInstanceLayerSupport(std::vector<const char*> instanceLayers)
    {
        uint32_t amountOfLayers = 0;
        VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
        evaluteVulkanResult(result);

        std::vector<VkLayerProperties> layers;
        layers.resize(amountOfLayers);
        result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());
        evaluteVulkanResult(result);

        uint32_t count = 0;

        for (uint32_t i = 0; i < instanceLayers.size(); i++)
        {
            for (uint32_t k = 0; k < amountOfLayers; k++)
            {
                if (strcmp(instanceLayers[i], layers[k].layerName) == 0)
                {
                    count++;
                    break;
                }
            }
        }

        if (count != instanceLayers.size())
            return false;

        return true;
    }

    VkExtent2D chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(Backend::g_window, &width, &height);

            VkExtent2D actualExtent =
            {
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    void cleanupImGui()
    {
        vkDeviceWaitIdle(device);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void cleanupSwapchain()
    {
        for (uint32_t i = 0; i < framebuffers.size(); i++)
            vkDestroyFramebuffer(device, framebuffers[i], pAllocator);

        for (uint32_t i = 0; i < imageViews.size(); i++)
            vkDestroyImageView(device, imageViews[i], pAllocator);

        vkDestroyImageView(device, depthImageView, pAllocator);
        vkDestroyImage(device, depthImage, pAllocator);
        vkFreeMemory(device, depthImageMemory, pAllocator);

        vkDestroySwapchainKHR(device, swapchain, pAllocator);
    }

    int cleanupVulkan()
    {
        vkDeviceWaitIdle(device);

        for (FrameData& frame : frames)
        {
            vkDestroySemaphore(device, frame.semaphoreRenderingDone, pAllocator);
            vkDestroySemaphore(device, frame.semaphoreImageAvailable, pAllocator);
            vkDestroyFence(device, frame.fenceCommandBufferDone, pAllocator);
            vkDestroyCommandPool(device, frame.commandPool, pAllocator);
            vkDestroyBuffer(device, frame.uniformBuffer.buffer, pAllocator);
            vkFreeMemory(device, frame.uniformBuffer.bufferMemory, pAllocator);
            vkDestroyBuffer(device, frame.objectBuffer.buffer, pAllocator);
            vkFreeMemory(device, frame.objectBuffer.bufferMemory, pAllocator);
        }

        cleanupSwapchain();

        vkDestroySampler(device, textureSampler, pAllocator);
        vkDestroyImageView(device, textureImageView, pAllocator);
        vkDestroyImage(device, textureImage, pAllocator);
        vkFreeMemory(device, textureImageMemory, pAllocator);

        vkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        vkDestroyRenderPass(device, renderPass, pAllocator);
        vkDestroyPipeline(device, pipeline, pAllocator);
        vkDestroyPipeline(device, wireframePipeline, pAllocator);

        vkDestroyDescriptorPool(device, descriptorPool, pAllocator);
        vkDestroyDescriptorPool(device, descriptorPoolImGui, pAllocator);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);

        vkDestroyBuffer(device, indexBuffer.buffer, pAllocator);
        vkFreeMemory(device, indexBuffer.bufferMemory, pAllocator);
        vkDestroyBuffer(device, vertexBuffer.buffer, pAllocator);
        vkFreeMemory(device, vertexBuffer.bufferMemory, pAllocator);

        vkDestroyDevice(device, pAllocator);
        vkDestroySurfaceKHR(instance, surface, pAllocator);

#ifdef DEBUG
        auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (vkDestroyDebugUtilsMessengerEXT != nullptr) {
            vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, pAllocator);
        }
#endif

        vkDestroyInstance(instance, pAllocator);

        return 0;
    }

    void copyBuffer(uint64_t size, VkBuffer srcBuffer, VkBuffer dstBuffer)
    {
        VkCommandBuffer commandBuffer;
        beginCommandBuffer(&commandBuffer);

        VkBufferCopy copyRegion =
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
        };

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endCommandBuffer(commandBuffer);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer;
        beginCommandBuffer(&commandBuffer);

        VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = { width, height, 1}
        };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        endCommandBuffer(commandBuffer);
    }

    void createBuffer(uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags properties, AllocatedBuffer& allocatedBuffer)
    {
        VkResult result;

        VkBufferCreateInfo bufferCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = usage,
            .sharingMode = sharingMode,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        result = vkCreateBuffer(device, &bufferCreateInfo, pAllocator, &allocatedBuffer.buffer);
        evaluteVulkanResult(result);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, allocatedBuffer.buffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = pickMemoryType(memoryRequirements.memoryTypeBits, properties)
        };

        result = vkAllocateMemory(device, &memoryAllocateInfo, pAllocator, &allocatedBuffer.bufferMemory);
        evaluteVulkanResult(result);

        result = vkBindBufferMemory(device, allocatedBuffer.buffer, allocatedBuffer.bufferMemory, 0);
        evaluteVulkanResult(result);
    }

    void createDepthResources()
    {
        VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

        VkImageCreateInfo imageCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthFormat,
            .extent = { static_cast<uint32_t>(g_windowSize.width), static_cast<uint32_t>(g_windowSize.height), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        createImage(imageCreateInfo, depthImage, depthImageMemory);

        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void createDescriptorPool()
    {
        VkResult result;

        VkDescriptorPoolSize descriptorPoolSize[] =
        {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = imageCount
            },
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = imageCount
            },
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = imageCount
            }
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = imageCount * IM_ARRAYSIZE(descriptorPoolSize),
            .poolSizeCount = (uint32_t)IM_ARRAYSIZE(descriptorPoolSize),
            .pPoolSizes = descriptorPoolSize
        };

        result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, pAllocator, &descriptorPool);
        evaluteVulkanResult(result);

        //For ImGui only Dont Touch
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets = 1000 * IM_ARRAYSIZE(pool_sizes),
            .poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes),
            .pPoolSizes = pool_sizes
        };

        result = vkCreateDescriptorPool(device, &pool_info, pAllocator, &descriptorPoolImGui);
        evaluteVulkanResult(result);
    }

    void createDescriptorSetLayout()
    {
        VkResult result;

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[] =
        {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = IM_ARRAYSIZE(descriptorSetLayoutBinding),
            .pBindings = descriptorSetLayoutBinding
        };

        result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, pAllocator, &descriptorSetLayout);
        evaluteVulkanResult(result);
    }

    void createDescriptorSets()
    {
        VkResult result;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptorSetLayout
        };

        for (FrameData& frameData : frames)
        {
            result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &frameData.descriptorSet);
            evaluteVulkanResult(result);

            VkDescriptorBufferInfo descriptorBufferInfo =
            {
                .buffer = frameData.uniformBuffer.buffer,
                .offset = 0,
                .range = sizeof(UniformBufferObject)
            };

            VkDescriptorImageInfo descriptorImageInfo =
            {
                .sampler = textureSampler,
                .imageView = textureImageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            VkDescriptorBufferInfo descriptorStorageBufferInfo =
            {
                .buffer = frameData.objectBuffer.buffer,
                .offset = 0,
                .range = sizeof(GameObjectData) * 1000
            };

            VkWriteDescriptorSet writeDescriptorSet[] =
            {
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = frameData.descriptorSet,
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &descriptorBufferInfo,
                    .pTexelBufferView = nullptr
                },
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = frameData.descriptorSet,
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &descriptorImageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr
                },
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = frameData.descriptorSet,
                    .dstBinding = 2,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &descriptorStorageBufferInfo,
                    .pTexelBufferView = nullptr
                }
            };

            vkUpdateDescriptorSets(device, 3, writeDescriptorSet, 0, nullptr);
        }
    }

    void createFramebuffers()
    {
        VkResult result;

        framebuffers.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            std::array<VkImageView, 2> attachments =
            {
                imageViews[i],
                depthImageView
            };

            VkFramebufferCreateInfo framebufferCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = renderPass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments = attachments.data(),
                .width = g_windowSize.width,
                .height = g_windowSize.height,
                .layers = 1
            };

            result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(framebuffers[i]));
            evaluteVulkanResult(result);
        }
    }

    void createFrameData()
    {
        VkResult result;

        VkSemaphoreCreateInfo semaphoreCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        VkFenceCreateInfo fenceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        //TODO: Check flags according to Vulkan Tutorial
        VkCommandPoolCreateInfo commandPoolCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamily.index.value()
        };

        frames.resize(imageCount);
        for (int i = 0; i < imageCount; i++)
        {
            frames[i] = { };
            // Semaphores
            result = vkCreateSemaphore(device, &semaphoreCreateInfo, pAllocator, &frames[i].semaphoreImageAvailable);
            evaluteVulkanResult(result);
            result = vkCreateSemaphore(device, &semaphoreCreateInfo, pAllocator, &frames[i].semaphoreRenderingDone);
            evaluteVulkanResult(result);

            // Fence
            result = vkCreateFence(device, &fenceCreateInfo, pAllocator, &frames[i].fenceCommandBufferDone);
            evaluteVulkanResult(result);

            // CommandPool
            result = vkCreateCommandPool(device, &commandPoolCreateInfo, pAllocator, &frames[i].commandPool);
            evaluteVulkanResult(result);

            { // CommandBuffer
                VkCommandBufferAllocateInfo commandBufferAllocateInfo =
                {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .commandPool = frames[i].commandPool,
                    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    .commandBufferCount = 1
                };

                result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &frames[i].mainCommandBuffer);
                evaluteVulkanResult(result);
            }
        }
    }

    void createGraphicsPipeline()
    {
        VkResult result;

        std::vector<char> shaderCodeVert, shaderCodeFrag;

        try
        {
            readFile("shader/vert.spv", shaderCodeVert);
            readFile("shader/frag.spv", shaderCodeFrag);
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            evaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
        }

        VkShaderModule shaderModuleVert, shaderModuleFrag;
        createShaderModule(shaderCodeVert, &shaderModuleVert);
        createShaderModule(shaderCodeFrag, &shaderModuleFrag);

        VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderModuleVert,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
            shaderStageCreateInfoFrag =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderModuleFrag,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        //TODO: Eventuell windowData als parameter �bergeben (konsistenz)
        //TODO: Viewport is never changed on resize, needs to maybe be recreated.
        VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(g_windowSize.width),
            .height = static_cast<float>(g_windowSize.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor =
        {
            .offset = { 0, 0 },
            .extent = g_windowSize
        };

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
        };

        // TODO: Very important for Triangle face
        VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo multisampleCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
        {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachmentState,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f
        };

        VkPipelineLayoutCreateInfo layoutCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        result = vkCreatePipelineLayout(device, &layoutCreateInfo, pAllocator, &pipelineLayout);
        evaluteVulkanResult(result);

        VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputCreateInfo,
            .pInputAssemblyState = &inputAssemblyCreateInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportStateCreateInfo,
            .pRasterizationState = &rasterizationCreateInfo,
            .pMultisampleState = &multisampleCreateInfo,
            .pDepthStencilState = &depthStencilStateCreateInfo,
            .pColorBlendState = &colorBlendCreateInfo,
            .pDynamicState = nullptr,
            .layout = pipelineLayout,
            .renderPass = renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, pAllocator, &pipeline);
        evaluteVulkanResult(result);

        // Wireframe Pipeline
        rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
        result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, pAllocator, &wireframePipeline);

        vkDestroyShaderModule(device, shaderModuleVert, nullptr);
        vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    }

    void createImage(const VkImageCreateInfo& imageCreateInfo, VkImage& image, VkDeviceMemory& imageMemory)
    {
        VkResult result;

        result = vkCreateImage(device, &imageCreateInfo, pAllocator, &image);
        evaluteVulkanResult(result);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = pickMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };

        result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory);
        evaluteVulkanResult(result);

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    VkImageView createImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags)
    {
        VkResult result;

        VkImageViewCreateInfo imageViewCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange =
            {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
        };

        VkImageView imageView;
        result = vkCreateImageView(device, &imageViewCreateInfo, pAllocator, &imageView);
        evaluteVulkanResult(result);

        return imageView;
    }

    void createImageViews()
    {
        VkResult result;

        result = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        evaluteVulkanResult(result);
        std::vector<VkImage> swapchainImages(imageCount);
        result = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
        evaluteVulkanResult(result);

        imageViews.resize(imageCount);

        for (uint32_t i = 0; i < imageCount; i++)
        {
            imageViews[i] = createImageView(swapchainImages[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void createIndexBuffer()
    {
        uint64_t bufferSize = sizeof(indices[0]) * indices.size();

        AllocatedBuffer stagingBuffer;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        void* data;
        vkMapMemory(device, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), bufferSize);
        vkUnmapMemory(device, stagingBuffer.bufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer);

        copyBuffer(bufferSize, stagingBuffer.buffer, indexBuffer.buffer);

        vkDestroyBuffer(device, stagingBuffer.buffer, pAllocator);
        vkFreeMemory(device, stagingBuffer.bufferMemory, pAllocator);
    }

    int createInstance()
    {
        VkResult result;

        VkApplicationInfo applicationInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "VulkanPrototype",
            .applicationVersion = VULKANPROTOTYPE_VERSION,
            .pEngineName = nullptr,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3
        };

        uint32_t amountOfGlfwExtensions = 0;
        const char** requiredGlfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);
        std::vector<const char*> instanceExtensions(requiredGlfwExtensions, requiredGlfwExtensions + amountOfGlfwExtensions);

        if (!checkInstanceExtensionSupport(instanceExtensions))
        {
            std::cout << "Extension not Supported!\n";
            return -1;
        }

#ifdef DEBUG

        std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };

        if (!checkInstanceLayerSupport(instanceLayers))
        {
            std::cout << "Validation Layer not Supported!\n";
            return -1;
        }

        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!checkInstanceExtensionSupport(instanceExtensions))
        {
            std::cout << "Extension not Supported!\n";
            return -1;
        }

        VkInstanceCreateInfo instanceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(instanceLayers.size()),
            .ppEnabledLayerNames = instanceLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
            .ppEnabledExtensionNames = instanceExtensions.data()
        };

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        evaluteVulkanResult(result);

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr
        };

        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (vkCreateDebugUtilsMessengerEXT == nullptr)
            return -1;

        result = vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
        evaluteVulkanResult(result);

#else

        VkInstanceCreateInfo instanceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
            .ppEnabledExtensionNames = instanceExtensions.data()
        };

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        evaluteVulkanResult(result);

#endif

        return 0;
    }

    void createLogicalDevice(VkPhysicalDevice physicalDevice)
    {
        VkResult result;

        queueFamily = pickQueueFamily(physicalDevice);

        std::vector<float> queuePriorities(queueFamily.queueCount);
        for (uint32_t i = 0; i < queueFamily.queueCount; i++)
            queuePriorities[i] = 1.0f;

        VkDeviceQueueCreateInfo deviceQueueCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamily.index.value(),
            .queueCount = queueFamily.queueCount,
            .pQueuePriorities = queuePriorities.data()
        };

        //TODO: Add a check if the features are available.
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
        physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;

        //TODO: Add a check if the Extensions are available.
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures =
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES,
            .pNext = nullptr,
            .shaderDrawParameters = VK_TRUE
        };

        VkDeviceCreateInfo deviceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &shaderDrawParametersFeatures,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &physicalDeviceFeatures
        };

        result = vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, &device);
        evaluteVulkanResult(result);

        vkGetDeviceQueue(device, queueFamily.index.value(), 0, &queue);
    }

    void createRenderPass()
    {
        VkResult result;

        VkAttachmentDescription colorAttachmentDescription =
        {
            .flags = 0,
            .format = surfaceFormat.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference colorAttachmentReference =
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkAttachmentDescription depthAttachmentDescription =
        {
            .flags = 0,
            .format = VK_FORMAT_D32_SFLOAT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        VkAttachmentReference depthAttachmentReference =
        {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpassDescription =
        {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentReference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = &depthAttachmentReference,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr
        };

        //TODO: Check if rendering is not done properly without this struct
        VkSubpassDependency subpassDependency =
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0
        };

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachmentDescription, depthAttachmentDescription };
        VkRenderPassCreateInfo renderPassCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 1,
            .pDependencies = &subpassDependency
        };

        result = vkCreateRenderPass(device, &renderPassCreateInfo, pAllocator, &renderPass);
        evaluteVulkanResult(result);
    }

    void createShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = shaderCode.size(),
            .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
        };

        VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, pAllocator, shaderModule);
        evaluteVulkanResult(result);
    }

    void createStorageBuffers()
    {
        uint64_t bufferSize = sizeof(GameObjectData) * 1000;

        for (FrameData& frameData : frames)
        {
            createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, frameData.objectBuffer);
        }
    }

    void createSwapchain(VkPhysicalDevice physicalDevice)
    {
        VkResult result;
        SurfaceDetails surfaceDetails = querySurfaceCapabilities(physicalDevice);

        surfaceFormat = chooseSurfaceFormat(surfaceDetails.formats);
        VkPresentModeKHR presentMode = choosePresentMode(surfaceDetails.presentModes);
        g_windowSize = chooseExtent2D(surfaceDetails.capabilities);

        uint32_t imageCount = surfaceDetails.capabilities.minImageCount + 1;

        if (surfaceDetails.capabilities.maxImageCount > 0 && imageCount > surfaceDetails.capabilities.maxImageCount)
        {
            imageCount = surfaceDetails.capabilities.maxImageCount;
        }

        //TODO: Parameter überprüfen
        VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surface,
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = g_windowSize,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = surfaceDetails.capabilities.currentTransform, //VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };

        result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, pAllocator, &swapchain);
        evaluteVulkanResult(result);
    }

    void createTextureImage()
    {
        int textureWidth, textureHeight, textureChannels;
        stbi_uc* pixels = stbi_load("assets/textures/texture.jpg", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

        VkDeviceSize imageSize = textureWidth * textureHeight * 4;

        if (!pixels) //TODO: Uncaracteristic Throw
        {
            throw std::runtime_error("failed to load texture image!");
        }

        AllocatedBuffer stagingBuffer;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        void* data;
        vkMapMemory(device, stagingBuffer.bufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBuffer.bufferMemory);

        stbi_image_free(pixels);

        VkImageCreateInfo imageCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .extent = { static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        createImage(imageCreateInfo, textureImage, textureImageMemory);
        transitionImageLayout(textureImage,  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer.buffer, textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
        transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device, stagingBuffer.buffer, pAllocator);
        vkFreeMemory(device, stagingBuffer.bufferMemory, pAllocator);
    }

    void createTextureImageView()
    {
        textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void createTextureSampler()
    {
        VkResult result;

        VkPhysicalDeviceProperties physicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSamplerCreateInfo samplerCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.f,
            .maxLod = 0.f,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        result = vkCreateSampler(device, &samplerCreateInfo, pAllocator, &textureSampler);
        evaluteVulkanResult(result);
    }

    void createUniformBuffers()
    {
        uint64_t bufferSize = sizeof(UniformBufferObject);

        for (uint64_t i = 0; i < imageCount; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, frames[i].uniformBuffer);
        }
    }

    void createVertexBuffer()
    {
        VkResult result;

        uint64_t bufferSize = sizeof(vertices[0]) * vertices.size();

        AllocatedBuffer stagingBuffer;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        void* data;
        result = vkMapMemory(device, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
        evaluteVulkanResult(result);
        memcpy(data, vertices.data(), bufferSize);
        vkUnmapMemory(device, stagingBuffer.bufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer);

        copyBuffer(bufferSize, stagingBuffer.buffer, vertexBuffer.buffer);

        vkDestroyBuffer(device, stagingBuffer.buffer, pAllocator);
        vkFreeMemory(device, stagingBuffer.bufferMemory, pAllocator);
    }

    int initializeImGui()
    {
        VkResult result;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = nullptr;

        io.Fonts->AddFontFromFileTTF("assets/font/DroidSans.ttf", 16 * Backend::GetMonitorScale());

        ImGui::StyleColorsDark();

        IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

        ImGui_ImplGlfw_InitForVulkan(Backend::g_window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.QueueFamily = queueFamily.index.value();
        init_info.Queue = queue;
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = descriptorPoolImGui;
        init_info.Subpass = 0;
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = pAllocator;
        init_info.CheckVkResultFn = evaluteVulkanResult;
        ImGui_ImplVulkan_Init(&init_info, renderPass);

        //Upload Fonts
        {
            // Use any command queue
            VkCommandPool command_pool = frames[0].commandPool;
            VkCommandBuffer command_buffer = frames[0].mainCommandBuffer;

            result = vkResetCommandPool(device, command_pool, 0);
            evaluteVulkanResult(result);
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            result = vkBeginCommandBuffer(command_buffer, &begin_info);
            evaluteVulkanResult(result);

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &command_buffer;
            result = vkEndCommandBuffer(command_buffer);
            evaluteVulkanResult(result);
            result = vkQueueSubmit(queue, 1, &end_info, VK_NULL_HANDLE);
            evaluteVulkanResult(result);

            result = vkDeviceWaitIdle(device);
            evaluteVulkanResult(result);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

        return 0;
    }

    int initializeVulkan()
    {
        VkResult result;

        if (createInstance() != 0)
            return -1;

        result = glfwCreateWindowSurface(instance, Backend::g_window, pAllocator, &surface);
        evaluteVulkanResult(result);

        physicalDevice = pickPhysicalDevice();

        createLogicalDevice(physicalDevice);

        VkBool32 surfaceSupport = false;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamily.index.value(), surface, &surfaceSupport);
        evaluteVulkanResult(result);

        if (!surfaceSupport)
        {
            std::cout << "Surface not Supported";
            evaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
            return -1;
        }

        createSwapchain(physicalDevice);
        createImageViews();
        createRenderPass();
        
        createDescriptorSetLayout();
        createGraphicsPipeline();

        createDepthResources();

        createFramebuffers();
        createFrameData();

        createTextureImage();
        createTextureImageView();
        createTextureSampler();

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createStorageBuffers();

        createDescriptorPool();
        createDescriptorSets();

        return 0;
    }

    uint32_t pickMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

        for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkPhysicalDevice pickPhysicalDevice()
    {
        uint32_t amountOfPhysicalDevices = 0;
        VkResult result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
        evaluteVulkanResult(result);

        std::vector<VkPhysicalDevice> physicalDevices(amountOfPhysicalDevices);
        result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
        evaluteVulkanResult(result);

        for (uint32_t i = 0; i < amountOfPhysicalDevices; i++)
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

            if (physicalDeviceProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                return physicalDevices[i];
            }
        }

        return physicalDevices[0];
    }

    QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice)
    {
        QueueFamily queueFamily;

        uint32_t amountOfQueueFamilyProperties = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(amountOfQueueFamilyProperties);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, queueFamilyProperties.data());

        for (uint32_t i = 0; i < amountOfQueueFamilyProperties; i++)
        {
            if (queueFamilyProperties[i].queueFlags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
            {
                queueFamily.index = i;
                queueFamily.queueCount = 1;

                break;
            }
        }

        if (!queueFamily.index.has_value())
            throw std::logic_error("No fitting QueueFamily was found."); //TODO: Inconsisten throw

        return queueFamily;
    }

    SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice)
    {
        SurfaceDetails surfaceDetails;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceDetails.capabilities);

        uint32_t amountOfSurfaceFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &amountOfSurfaceFormats, nullptr);
        surfaceDetails.formats.resize(amountOfSurfaceFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &amountOfSurfaceFormats, surfaceDetails.formats.data());

        uint32_t amountOFPresentModes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &amountOFPresentModes, nullptr);
        surfaceDetails.presentModes.resize(amountOFPresentModes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &amountOFPresentModes, surfaceDetails.presentModes.data());

        return surfaceDetails;
    }

    void recreateGraphicsPipelineAndSwapchain()
    {
        vkDeviceWaitIdle(device);

        cleanupSwapchain();

        vkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        vkDestroyPipeline(device, pipeline, pAllocator);
        vkDestroyPipeline(device, wireframePipeline, pAllocator);

        int width = 0, height = 0;
        glfwGetFramebufferSize(Backend::g_window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(Backend::g_window, &width, &height);
            glfwWaitEvents();
        }

        createSwapchain(physicalDevice);
        createImageViews();

        createGraphicsPipeline();
        
        createDepthResources();
        createFramebuffers();
    }

    void updateUniformBuffer(uint32_t imageIndex)
    {
        // static auto startTime = std::chrono::high_resolution_clock::now();

        // auto currentTime = std::chrono::high_resolution_clock::now();
        // float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto x = g_uboValues.center + g_uboValues.eye;

        UniformBufferObject ubo =
        {
            .model = glm::translate(glm::mat4(1.0f), g_uboValues.axis),
            .view = glm::lookAt(g_uboValues.eye, g_uboValues.center + g_uboValues.eye, g_uboValues.up),
            .proj = glm::perspective(glm::radians(g_uboValues.fovy), static_cast<float>(g_windowSize.width) / static_cast<float>(g_windowSize.height), g_uboValues.near, g_uboValues.far)
        };

        /*UniformBufferObject ubo =
        {
            .model = glm::rotate(glm::mat4(1.0f), glm::radians(g_uboValues.angle), glm::vec3(1.0f, 0.0f, 0.0f)),
            .view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.5f)),
            .proj = glm::perspective(glm::radians(60.0f), static_cast<float>(g_windowSize.width) / static_cast<float>(g_windowSize.height), 0.1f, 10.0f)
        };*/

        //UniformBufferObject ubo =
        //{
        //    .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        //    .view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        //    .proj = glm::perspective(glm::radians(60.0f), static_cast<float>(windowSize.width) / static_cast<float>(windowSize.height), 0.1f, 10.0f)
        //};

        ubo.proj[1][1] *= -1;

        {
            void* data;
            vkMapMemory(device, frames[imageIndex].uniformBuffer.bufferMemory, 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
            vkUnmapMemory(device, frames[imageIndex].uniformBuffer.bufferMemory);
        }

        {
            void* data;
            vkMapMemory(device, frames[imageIndex].objectBuffer.bufferMemory, 0, sizeof(ubo), 0, &data);

            GameObjectData* gameObjectData = (GameObjectData*)data;

            gameObjectData[0].globalPosition = glm::vec3(-1, 1, -1);
            gameObjectData[1].globalPosition = glm::vec3(0, 1, 0);
            gameObjectData[2].globalPosition = glm::vec3(1, 1, 1);
            gameObjectData[3].globalPosition = glm::vec3(-1, 0, -1);
            gameObjectData[4].globalPosition = glm::vec3(0, 0, 0);
            gameObjectData[5].globalPosition = glm::vec3(1, 0, 1);
            gameObjectData[6].globalPosition = glm::vec3(-1, -1, -1);
            gameObjectData[7].globalPosition = glm::vec3(0, -1, 0);
            gameObjectData[8].globalPosition = glm::vec3(1, -1, 1);

            vkUnmapMemory(device, frames[imageIndex].objectBuffer.bufferMemory);
        }
    }

    /*
     * Global Functions
     */

    void Cleanup()
    {
        cleanupImGui();
        cleanupVulkan();
    }

    int Initialize()
    {
        initializeVulkan();
        initializeImGui();

        return 0;
    }

    void RenderFrame(ImDrawData* draw_data)
    {
        static uint32_t imageIndex = 0;
        static uint32_t frameNumber = 0;
        VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frames[frameNumber].semaphoreImageAvailable, nullptr, &imageIndex);
        evaluteVulkanResult(result);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateGraphicsPipelineAndSwapchain();
            return;
        }

        // wait indefinitely instead of periodically checking
        result = vkWaitForFences(device, 1, &frames[frameNumber].fenceCommandBufferDone, VK_TRUE, UINT64_MAX);
        evaluteVulkanResult(result);

        result = vkResetFences(device, 1, &frames[frameNumber].fenceCommandBufferDone);
        evaluteVulkanResult(result);

        /*result = vkResetCommandPool(device, commandPool, 0);
        evaluteVulkanResult(result);*/

        result = vkResetCommandBuffer(frames[frameNumber].mainCommandBuffer, 0);
        evaluteVulkanResult(result);

        {
            VkCommandBufferBeginInfo info =
            {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = nullptr
            };

            result = vkBeginCommandBuffer(frames[frameNumber].mainCommandBuffer, &info);
            evaluteVulkanResult(result);
        }

        { 
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
            clearValues[1].depthStencil = { 1.0f, 0 };

            VkRenderPassBeginInfo renderPassBeginInfo =
            {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = renderPass,
                .framebuffer = framebuffers[imageIndex],
                .renderArea = {{0, 0}, g_windowSize},
                .clearValueCount = static_cast<uint32_t>(clearValues.size()),
                .pClearValues = clearValues.data()
            };

            vkCmdBeginRenderPass(frames[frameNumber].mainCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        }

        updateUniformBuffer(imageIndex);

        VkPipeline graphicsPipeline = g_polygonMode == VK_POLYGON_MODE_FILL ? pipeline : wireframePipeline;
        vkCmdBindPipeline(frames[frameNumber].mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkBuffer vertexBuffers[] = { vertexBuffer.buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(frames[frameNumber].mainCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(frames[frameNumber].mainCommandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(frames[frameNumber].mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frames[imageIndex].descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 1);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 2);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 3);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 4);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 5);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 6);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 7);
        vkCmdDrawIndexed(frames[frameNumber].mainCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 8);

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, frames[frameNumber].mainCommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(frames[frameNumber].mainCommandBuffer);

        VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &frames[frameNumber].semaphoreImageAvailable,
            .pWaitDstStageMask = waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &frames[frameNumber].mainCommandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &frames[frameNumber].semaphoreRenderingDone
        };

        vkEndCommandBuffer(frames[frameNumber].mainCommandBuffer);

        vkQueueSubmit(queue, 1, &submitInfo, frames[frameNumber].fenceCommandBufferDone);

        VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &frames[frameNumber].semaphoreRenderingDone,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr
        };

        result = vkQueuePresentKHR(queue, &presentInfo);
        evaluteVulkanResult(result);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            recreateGraphicsPipelineAndSwapchain();
        }

        frameNumber = (frameNumber + 1) % imageCount;
    }
}