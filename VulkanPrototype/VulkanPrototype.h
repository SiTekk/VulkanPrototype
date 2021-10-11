#ifndef VULKANPROTOTYPE_H
#define VULKANPROTOTYPE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <iostream>
#include <vector>

#define VULKANPROTOTYPE_VERSION VK_MAKE_VERSION(0, 1, 0)

namespace VulkanPrototype
{
    class VulkanPrototype
    {

    public:
        /// <summary>
        /// Konstruktor f�r die VulkanProtoype Klasse.
        /// </summary>
        VulkanPrototype();

        /// <summary>
        /// Evaluiert das Vulkan Result, gibt eine Fehlermeldung aus und Pausiert im Debugger das Program.
        /// </summary>
        /// <param name="result">Die VkResult Struktur welche Evaluiert werden soll.</param>
        void EvaluteVulkanResult(VkResult result);

        /// <summary>
        /// Hauptmethode, die zu Beginn des Programms aufgerufen werden muss. Hier werden alle Funktionen f�r die Initialisierung aufgerufen.
        /// </summary>
        /// <returns>Gibt nach erfolgreichem beenden 0 zur�ck.</returns>
        int Run();

        /// <summary>
        /// Die GLFWwindow instanz, welche die gr��e und die Events des Fensters enth�lt.
        /// </summary>
        GLFWwindow *Window;
        VkDevice Device;
        VkInstance Instance;
        VkQueue Queue;
        VkSurfaceKHR Surface;

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
        /// L�scht die nicht mehr ben�tigten GLFW Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupGlfw();

        /// <summary>
        /// L�scht die nicht mehr ben�tigten Vulkan Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupVulkan();

        /// <summary>
        /// Initialisiert die ben�tigten GLFW Komponenten.
        /// </summary>
        int initializeGlfw();

        /// <summary>
        /// Initialisiert die ben�tigtren Vulkan Komponenten.
        /// </summary>
        int initializeVulkan();

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

        struct uSize
        {
            uint32_t x, y;
        } windowSize;
    };
}

#endif
