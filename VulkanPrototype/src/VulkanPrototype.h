#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_vulkan.h>

#include <glm/glm.hpp>

#include <array>
#include <algorithm>
//#include <cstring>
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
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
        static VkVertexInputBindingDescription getBindingDescription();
    };

    class VulkanPrototype
    {

    public:

        VulkanPrototype();

        /// <summary>
        /// Hauptmethode, die zu Beginn des Programms aufgerufen werden muss. Hier werden alle Funktionen für die Initialisierung aufgerufen.
        /// </summary>
        /// <returns>Gibt nach erfolgreichem beenden 0 zurück.</returns>
        int Run();

    private:

        bool checkInstanceExtensionSupport(std::vector<const char*> instanceExtensions);
        bool checkInstanceLayerSupport(std::vector<const char *> instanceLayers);

        VkExtent2D chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities);
        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        int cleanupGlfw();
        void cleanupSwapchain();
        int cleanupVulkan();

        void copyBuffer(uint64_t size, VkBuffer srcBuffer, VkBuffer dstBuffer);

        void createBuffer(uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createCommandBuffers(ImGui_ImplVulkanH_Window& wd);
        void createCommandPool(ImGui_ImplVulkanH_Window& wd);
        void createFramebuffers(ImGui_ImplVulkanH_Window& wd);
        void createGraphicsPipeline(ImGui_ImplVulkanH_Window& wd);
        void createImageViews(ImGui_ImplVulkanH_Window& wd);
        void createIndexBuffer();
        int createInstance();
        void createLogicalDevice(VkPhysicalDevice physicalDevice);
        void createRenderPass(ImGui_ImplVulkanH_Window& wd);
        void createSemaphores();
        void createShaderModule(const std::vector<char>& shaderCode, VkShaderModule *shaderModule);
        void createSwapchain(VkPhysicalDevice physicalDevice, ImGui_ImplVulkanH_Window& wd);
        void createVertexBuffer();

        void drawFrame();

        int initializeGlfw();
        int initializeVulkan();

        int mainLoop();

        uint32_t pickMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkPhysicalDevice pickPhysicalDevice();
        QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice);

        //TODO: Maybe pass SurfaceDetails as rederence: void querySurfaceCapabilities(VkPhysicalDevice physicalDevice, SurfaceDetails& surfaceDetails)
        SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice, ImGui_ImplVulkanH_Window& wd);

        void readFile(const std::string& filename, std::vector<char>& buffer);

        void recreateSwapchain();

        void recordCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, std::vector<VkFramebuffer>& framebuffers, ImGui_ImplVulkanH_Window& wd);

    private:

        GLFWwindow* window;
        ImGui_ImplVulkanH_Window windowData;

        //TODO: Check if members should be outsourced to the imgui structs
        //ImGui_ImplVulkanH_Frame vulkanFrames;
        //ImGui_ImplVulkanH_FrameSemaphores frameSemaphores;

        //Buffer
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkCommandPool commandPool;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDevice device;
        VkExtent2D swapchainExtent;
        VkFence fenceInFlight;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkPipelineLayout pipelineLayout;
        VkQueue queue;
        //TODO: Check if multiple Semaphores should be used (Same amount as Command / Frame Buffers)
        VkSemaphore semaphoreImageAvailable, semaphoreRenderingDone;

        QueueFamily queueFamily;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkImageView> imageViews;
    };
}

#endif
