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
        /// Konstruktor für die VulkanProtoype Klasse.
        /// </summary>
        VulkanPrototype();

        /// <summary>
        /// Evaluiert das Vulkan Result, gibt eine Fehlermeldung aus und Pausiert im Debugger das Program.
        /// </summary>
        /// <param name="result">Die VkResult Struktur welche Evaluiert werden soll.</param>
        void EvaluteVulkanResult(VkResult result);

        /// <summary>
        /// Hauptmethode, die zu Beginn des Programms aufgerufen werden muss. Hier werden alle Funktionen für die Initialisierung aufgerufen.
        /// </summary>
        /// <returns>Gibt nach erfolgreichem beenden 0 zurück.</returns>
        int Run();

        /// <summary>
        /// Die GLFWwindow instanz, welche die größe und die Events des Fensters enthält.
        /// </summary>
        GLFWwindow *Window;
        VkDevice Device;
        VkInstance Instance;
        VkQueue Queue;
        VkSurfaceKHR Surface;

    private:

        /// <summary>
        /// Überprüft ob die Übergebenen Extensions unterstützt werden.
        /// </summary>
        /// <param name="instanceExtensions">Der Vektor mit den Namen der Extensions</param>
        bool checkInstanceExtensions(std::vector<const char*> instanceExtensions);

        /// <summary>
        /// Überprüft ob die übergebenen Layers unterstützt werden.
        /// </summary>
        /// <param name="instanceLayers">Der Vektor mit den Namen der Layers</param>
        bool checkInstanceLayerSupport(std::vector<const char *> instanceLayers);

        /// <summary>
        /// Löscht die nicht mehr benötigten GLFW Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupGlfw();

        /// <summary>
        /// Löscht die nicht mehr benötigten Vulkan Komponenten und gibt den Arbeitsspeicher wieder frei.
        /// </summary>
        int cleanupVulkan();

        /// <summary>
        /// Initialisiert die benötigten GLFW Komponenten.
        /// </summary>
        int initializeGlfw();

        /// <summary>
        /// Initialisiert die benötigtren Vulkan Komponenten.
        /// </summary>
        int initializeVulkan();

        /// <summary>
        /// Die Hauptschleife in der die Events und die Spiellogik verarbeitet werden.
        /// </summary>
        int mainLoop();

        /// <summary>
        /// Sucht die Graphikkarte mit der besten Performance aus und gibt diese zurück.
        /// </summary>
        VkPhysicalDevice pickPhysicalDevice();

        /// <summary>
        /// Sucht die geeignete QueueFamily aus und gibt die entsprechende Info dafür zurück.
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
