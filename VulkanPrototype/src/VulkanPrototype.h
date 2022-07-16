#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <optional>

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
        /// Hauptmethode, die zu Beginn des Programms aufgerufen werden muss. Hier werden alle Funktionen f�r die Initialisierung aufgerufen.
        /// </summary>
        /// <returns>Gibt nach erfolgreichem beenden 0 zur�ck.</returns>
        int Run();

    private:

        bool checkInstanceExtensionSupport(std::vector<const char*> instanceExtensions);
        bool checkInstanceLayerSupport(std::vector<const char *> instanceLayers);

        int cleanupGlfw();
        int cleanupVulkan();

        int createInstance();
        void createLogicalDevice(VkPhysicalDevice physicalDevice);
        void createShaderModule(const std::vector<char>& shaderCodeVert, VkShaderModule *shaderModule);

        void drawFrame();

        int initializeGlfw();
        int initializeVulkan();

        int mainLoop();

        VkPhysicalDevice pickPhysicalDevice();
        QueueFamily pickQueueFamily(VkPhysicalDevice physicalDevice);

        SurfaceDetails querySurfaceCapabilities(VkPhysicalDevice physicalDevice);

        std::vector<char> readFile(const std::string& filename);

    private:

        GLFWwindow* window;

        WindowSpecification windowSize;

        VkCommandPool commandPool;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDevice device;
        VkFormat imageFormat;
        VkInstance instance;
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        VkQueue queue;
        VkRenderPass renderPass;
        VkSemaphore semaphoreImageAvailable, semaphoreRenderingDone;
        VkShaderModule shaderModuleVert, shaderModuleFrag;
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;

        QueueFamily queueFamily;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> frameBuffers;
        std::vector<VkImageView> imageViews;
    };
}

#endif
