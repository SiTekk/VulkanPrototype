#include "VulkanPrototype.h"

namespace VulkanPrototype
{
    VulkanPrototype::VulkanPrototype()
    {
        windowSize = {.x = 1280, .y = 720};

        //Werden in Separaten Funktionen initialisiert
        Window = nullptr;
        Device = nullptr;
        Instance = nullptr;
        Queue = nullptr;
        Surface = nullptr;
    }

    void VulkanPrototype::EvaluteVulkanResult(VkResult result)
    {
        if (result != VK_SUCCESS)
        {
#ifdef _WIN32
            __debugbreak();
#endif
            std::cout << result;
        }
    }

    int VulkanPrototype::Run()
    {
        initializeGlfw();
        initializeVulkan();
        mainLoop();
        cleanupVulkan();
        cleanupGlfw();
        return 0;
    }

    bool VulkanPrototype::checkInstanceExtensions(std::vector<const char*> instanceExtensions)
    {
        uint32_t amountOfExtensions = 0;
        VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
        EvaluteVulkanResult(result);

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
        else
            return true;
    }

    bool VulkanPrototype::checkInstanceLayerSupport(std::vector<const char *> instanceLayers)
    {
        uint32_t amountOfLayers = 0;
        VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
        EvaluteVulkanResult(result);

        std::vector<VkLayerProperties> layers;
        layers.resize(amountOfLayers);
        result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());
        EvaluteVulkanResult(result);

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
        else
            return true;

        return true;
    }

    int VulkanPrototype::cleanupGlfw()
    {
        glfwDestroyWindow(Window);
        return 0;
    }

    int VulkanPrototype::cleanupVulkan()
    {
        vkDeviceWaitIdle(Device);

        vkDestroyDevice(Device, nullptr);
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);

        return 0;
    }

    int VulkanPrototype::initializeGlfw()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Window = glfwCreateWindow(windowSize.x, windowSize.y, "VulkanPrototype", nullptr, nullptr);

        return 0;
    }

    int VulkanPrototype::initializeVulkan()
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
            .apiVersion = VK_API_VERSION_1_2
        };

        std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };

        if (!checkInstanceLayerSupport(instanceLayers))
        {
            std::cout << "Validation Layer not Supported!\n";
            return -1;
        }

        uint32_t amountOfGlfwExtensions = 0;
        const char** requiredGlfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);
        std::vector<const char*> instanceExtensions;
        instanceExtensions.resize(amountOfGlfwExtensions);
        instanceExtensions.assign(requiredGlfwExtensions, requiredGlfwExtensions + amountOfGlfwExtensions);

        if (!checkInstanceExtensions(instanceExtensions))
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

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);
        EvaluteVulkanResult(result);

        result = glfwCreateWindowSurface(Instance, Window, nullptr, &Surface);
        EvaluteVulkanResult(result);

        VkPhysicalDevice physicalDevice = pickPhysicalDevice();
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkDeviceQueueCreateInfo deviceQueueCreateInfo = pickQueueFamily(physicalDevice);

        std::vector<float> queuePriorities;
        queuePriorities.resize(deviceQueueCreateInfo.queueCount);
        for (uint32_t i = 0; i < deviceQueueCreateInfo.queueCount; i++)
            queuePriorities[i] = 1.0f;
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();

        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

        VkDeviceCreateInfo deviceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = nullptr,
            .pEnabledFeatures = &physicalDeviceFeatures
        };

        result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &Device);
        EvaluteVulkanResult(result);

        vkGetDeviceQueue(Device, 0, 0, &Queue);

        return 0;
    }

    int VulkanPrototype::mainLoop()
    {
        while (!glfwWindowShouldClose(Window))
        {
            glfwPollEvents();
        }

        return 0;
    }

    VkPhysicalDevice VulkanPrototype::pickPhysicalDevice()
    {
        uint32_t amountOfPhysicalDevices = 0;
        VkResult result = vkEnumeratePhysicalDevices(Instance, &amountOfPhysicalDevices, nullptr);
        EvaluteVulkanResult(result);

        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(amountOfPhysicalDevices);
        result = vkEnumeratePhysicalDevices(Instance, &amountOfPhysicalDevices, physicalDevices.data());
        EvaluteVulkanResult(result);

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

    VkDeviceQueueCreateInfo VulkanPrototype::pickQueueFamily(VkPhysicalDevice physicalDevice)
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        uint32_t amountOfQueueFamilyProperties = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        queueFamilyProperties.resize(amountOfQueueFamilyProperties);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, queueFamilyProperties.data());

        for (uint32_t i = 0; i < amountOfQueueFamilyProperties; i++)
        {
            if (queueFamilyProperties[i].queueFlags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
            {
                deviceQueueCreateInfo.queueFamilyIndex = i;

                if (queueFamilyProperties[i].queueCount < 4)
                {
                    deviceQueueCreateInfo.queueCount = queueFamilyProperties[i].queueCount;
                }
                else
                {
                    deviceQueueCreateInfo.queueCount = 4;
                }
                break;
            }
        }

        if (deviceQueueCreateInfo.queueCount == 0)
            throw std::logic_error("No fitting QueueFamily was found.");

        return deviceQueueCreateInfo;
    }
}