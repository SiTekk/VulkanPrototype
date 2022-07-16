#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
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

    struct WindowSpecification
    {
        uint32_t width, height;
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
        int cleanupVulkan();

        int createInstance();
        void createLogicalDevice(VkPhysicalDevice physicalDevice);
        void createShaderModule(const std::vector<char>& shaderCodeVert, VkShaderModule *shaderModule);
        void createSwapchain(VkPhysicalDevice physicalDevice);

        void drawFrame();

        int initializeGlfw();
        int initializeVulkan();

        int mainLoop();

        VkPhysicalDevice pickPhysicalDevice();
        QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice);


        //TODO: Maybe pass SurfaceDetails as rederence: void querySurfaceCapabilities(VkPhysicalDevice physicalDevice, SurfaceDetails& surfaceDetails)
        SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice);

        std::vector<char> readFile(const std::string& filename);

    private:

        GLFWwindow* window;

        WindowSpecification windowSize;

        VkCommandPool commandPool;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDevice device;
        VkExtent2D swapchainExtent;
        VkInstance instance;
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        VkQueue queue;
        VkRenderPass renderPass;
        VkSemaphore semaphoreImageAvailable, semaphoreRenderingDone;
        VkShaderModule shaderModuleVert, shaderModuleFrag;
        VkSurfaceKHR surface;
        VkSurfaceFormatKHR surfaceFormat;
        VkSwapchainKHR swapchain;

        QueueFamily queueFamily;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> frameBuffers;
        std::vector<VkImageView> imageViews;
    };
}

#endif
