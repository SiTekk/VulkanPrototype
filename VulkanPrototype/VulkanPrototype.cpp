#include "VulkanPrototype.h"

namespace VulkanPrototype
{
    VulkanPrototype::VulkanPrototype()
    {
        windowSize = {.width = 1280, .height = 720};

        //Werden in Separaten Funktionen initialisiert
        Window = nullptr;
        Device = nullptr;
        Instance = nullptr;
        PipelineLayout = nullptr;
        Queue = nullptr;
        Surface = nullptr;
        Swapchain = nullptr;
        ShaderModuleFrag = nullptr;
        ShaderModuleVert = nullptr;
    }

    void VulkanPrototype::EvaluteVulkanResult(VkResult result)
    {
        if (result != VK_SUCCESS)
        {
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

    void VulkanPrototype::checkSurfaceCapabilities(VkPhysicalDevice physicalDevice)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, Surface, &surfaceCapabilities);

        uint32_t amountOfSurfaceFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, Surface, &amountOfSurfaceFormats, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        surfaceFormats.resize(amountOfSurfaceFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, Surface, &amountOfSurfaceFormats, surfaceFormats.data());

        uint32_t amountOFPresentModes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, Surface, &amountOFPresentModes, nullptr);
        std::vector<VkPresentModeKHR> presentModes;
        presentModes.resize(amountOFPresentModes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, Surface, &amountOFPresentModes, presentModes.data());

        return;
    }

    int VulkanPrototype::cleanupGlfw()
    {
        glfwDestroyWindow(Window);
        return 0;
    }

    int VulkanPrototype::cleanupVulkan()
    {
        vkDeviceWaitIdle(Device);

        vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
        vkDestroyShaderModule(Device, ShaderModuleVert, nullptr);
        vkDestroyShaderModule(Device, ShaderModuleFrag, nullptr);
        for (uint32_t i = 0; i < ImageViews.size(); i++)
            vkDestroyImageView(Device, ImageViews[i], nullptr);
        vkDestroySwapchainKHR(Device, Swapchain, nullptr);
        vkDestroyDevice(Device, nullptr);
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);

        return 0;
    }

    void VulkanPrototype::createShaderModule(const std::vector<char>& shaderCodeVert, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = shaderCodeVert.size(),
            .pCode = (uint32_t*)shaderCodeVert.data()
        };

        VkResult result = vkCreateShaderModule(Device, &shaderModuleCreateInfo, nullptr, shaderModule);
        EvaluteVulkanResult(result);
    }

    int VulkanPrototype::initializeGlfw()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        Window = glfwCreateWindow(windowSize.width, windowSize.height, "VulkanPrototype", nullptr, nullptr);

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
            .apiVersion = VK_API_VERSION_1_3
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

        //TODO: Device Extensions überprüfen.
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo deviceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &physicalDeviceFeatures
        };

        result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &Device);
        EvaluteVulkanResult(result);

        vkGetDeviceQueue(Device, 0, 0, &Queue);

        VkBool32 surfaceSupport = false;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, Surface, &surfaceSupport);
        EvaluteVulkanResult(result);

        if (!surfaceSupport)
        {
            std::cout << "Surface not Supported";
            EvaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
            return -1;
        }

        checkSurfaceCapabilities(physicalDevice);

        //TODO: Parameter Überprüfen
        VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = Surface,
            .minImageCount = 3,
            .imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = VkExtent2D {windowSize.width, windowSize.height },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };

        result = vkCreateSwapchainKHR(Device, &swapchainCreateInfo, nullptr, &Swapchain);
        EvaluteVulkanResult(result);

        uint32_t amountOfImagesInSwapchain = 0;
        result = vkGetSwapchainImagesKHR(Device, Swapchain, &amountOfImagesInSwapchain, nullptr);
        EvaluteVulkanResult(result);
        std::vector<VkImage> swapchainImages;
        swapchainImages.resize(amountOfImagesInSwapchain);
        result = vkGetSwapchainImagesKHR(Device, Swapchain, &amountOfImagesInSwapchain, swapchainImages.data());
        EvaluteVulkanResult(result);

        ImageViews.resize(amountOfImagesInSwapchain);

        for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++)
        {

            VkImageViewCreateInfo imageViewCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = swapchainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchainCreateInfo.imageFormat,
                .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            result = vkCreateImageView(Device, &imageViewCreateInfo, nullptr, &ImageViews[i]);
            EvaluteVulkanResult(result);
        }

        std::vector<char> shaderCodeVert, shaderCodeFrag;

        try
        {
            shaderCodeVert = readFile("vert.spv");
            shaderCodeFrag = readFile("frag.spv");
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            EvaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
        }

        createShaderModule(shaderCodeVert, &ShaderModuleVert);
        createShaderModule(shaderCodeFrag, &ShaderModuleFrag);

        VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = ShaderModuleVert,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        shaderStageCreateInfoFrag = 
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = ShaderModuleFrag,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)windowSize.width,
            .height = (float)windowSize.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor =
        {
            .offset = { 0, 0 },
            .extent = { windowSize.width, windowSize.height }
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

        VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
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

        VkPipelineLayoutCreateInfo layoutCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        result = vkCreatePipelineLayout(Device, &layoutCreateInfo, nullptr, &PipelineLayout);
        EvaluteVulkanResult(result);

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

    std::vector<char> VulkanPrototype::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (file)
        {
            size_t fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0);
            std::vector<char> fileBuffer(fileSize);
            file.read(fileBuffer.data(), fileSize);
            file.close();

            return fileBuffer;
        }
        else
        {
            throw std::runtime_error("Datei \"" + filename + "\" konnte nicht geöffnet werden!");
        }
    }
}