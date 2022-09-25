#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
        VkSurfaceCapabilitiesKHR capabilities;
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
        void createDescriptorPool(ImGui_ImplVulkanH_Window& wd);
        void createDescriptorSetLayout();
        void createDescriptorSets();
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
        void createUniformBuffers(ImGui_ImplVulkanH_Window& wd);
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

        void updateUniformBuffer(uint32_t imageIndex, ImGui_ImplVulkanH_Window& wd);

    private:

        GLFWwindow* window;
        
        //TODO: Replace ImGui_Impl with custom struct and add Extent2d
        ImGui_ImplVulkanH_Window windowData;

        //TODO: Check if members should be outsourced to the imgui structs
        //ImGui_ImplVulkanH_Frame vulkanFrames;
        //ImGui_ImplVulkanH_FrameSemaphores frameSemaphores;

        //Buffers
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        //Descriptors
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        VkAllocationCallbacks *pAllocator = nullptr;
        VkCommandPool commandPool;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDevice device;
        //VkExtent2D swapchainExtent; //Should be added to window data
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
