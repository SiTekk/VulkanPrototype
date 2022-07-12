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

        /// <summary>
        /// �berpr�ft ob die �bergebenen Extensions unterst�tzt werden.
        /// </summary>
        /// <param name="instanceExtensions">Der Vektor mit den Namen der Extensions</param>
        bool checkInstanceExtensions(std::vector<const char*> instanceExtensions);

        /// <summary>
        /// �berpr�ft ob die �bergebenen Layers unterst�tzt werden.
        /// </summary>
        /// <param name="instanceLayers">Der Vektor mit den Namen der Layers</param>
        bool checkInstanceLayerSupport(std::vector<const char *> instanceLayers);

        /// <summary>
        /// �berpr�ft die Capabiliteis des Surfaces.
        /// </summary>
        void checkSurfaceCapabilities(VkPhysicalDevice physicalDevice);

        /// <summary>
        /// L�scht die nicht mehr ben�tigten GLFW Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupGlfw();

        /// <summary>
        /// L�scht die nicht mehr ben�tigten Vulkan Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupVulkan();

        void createShaderModule(const std::vector<char>& shaderCodeVert, VkShaderModule *shaderModule);

        void drawFrame();

        /// <summary>
        /// Evaluiert das Vulkan Result, gibt eine Fehlermeldung aus und Pausiert im Debugger das Program.
        /// </summary>
        /// <param name="result">Die VkResult Struktur welche Evaluiert werden soll.</param>
        void evaluteVulkanResult(VkResult result);

        /// <summary>
        /// Initialisiert die ben�tigten GLFW Komponenten.
        /// </summary>
        int initializeGlfw();

        /// <summary>
        /// Initialisiert die ben�tigtren Vulkan Komponenten.
        /// </summary>
        int initializeVulkan();

        int createInstance();

        /// <summary>
        /// Die Hauptschleife in der die Events und die Spiellogik verarbeitet werden.
        /// </summary>
        int mainLoop();

        /// <summary>
        /// Sucht die Graphikkarte mit der besten Performance aus und gibt diese zur�ck.
        /// </summary>
        VkPhysicalDevice pickPhysicalDevice();

        /// <summary>
        /// Sucht die geeignete QueueFamily aus und gibt die entsprechende Info daf�r zur�ck.
        /// </summary>
        /// <param name="physicalDevice">Das PhysicalDevice dessen QueueFamilies untersucht werden soll.</param>
        VkDeviceQueueCreateInfo pickQueueFamily(VkPhysicalDevice physicalDevice);

        std::vector<char> readFile(const std::string& filename);

        struct uSize
        {
            uint32_t width, height;
        } windowSize;

        GLFWwindow* window;
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
