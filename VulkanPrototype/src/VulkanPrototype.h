#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>

#define VULKANPROTOTYPE_VERSION VK_MAKE_VERSION(0, 1, 0)

namespace VulkanPrototype
{
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
        void checkSurfaceCapabilities(VkPhysicalDevice physicalDevice);

        int cleanupGlfw();
        int cleanupVulkan();

        int createInstance();
        void createShaderModule(const std::vector<char>& shaderCodeVert, VkShaderModule *shaderModule);

        void drawFrame();

        void evaluteVulkanResult(VkResult result);

        int initializeGlfw();
        int initializeVulkan();

        int mainLoop();

        VkPhysicalDevice pickPhysicalDevice();
        VkDeviceQueueCreateInfo pickQueueFamily(VkPhysicalDevice physicalDevice);

        std::vector<char> readFile(const std::string& filename);

    private:

        GLFWwindow* window;

        WindowSpecification windowSize;

        VkCommandPool commandPool;
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

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> frameBuffers;
        std::vector<VkImageView> imageViews;
    };
}

#endif
