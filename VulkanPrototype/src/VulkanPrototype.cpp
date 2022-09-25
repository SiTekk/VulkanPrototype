#include "VulkanPrototype.h"

namespace VulkanPrototype
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    static void evaluteVulkanResult(VkResult result)
    {
        //TODO: Implement correct Program abortion
        if (result != VK_SUCCESS)
        {
            std::cout << result << "\n";
        }
    }

    static const std::vector<Vertex> vertices = 
    {
        {{-1.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}},

        {{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{1.0f, -0.5f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
    };

    static const std::vector<uint16_t> indices =
    {
        0, 1, 2, 2, 3, 0, 4, 5, 6
    };

    std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

    VkVertexInputBindingDescription Vertex::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = 
        {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        return bindingDescription;
    }

    VulkanPrototype::VulkanPrototype()
    {
        windowData = {};
        windowData.Width = 1600;
        windowData.Height = 900;

        window = nullptr;
        commandPool = nullptr;
        debugMessenger = nullptr;
        device = nullptr;
        instance = nullptr;
        physicalDevice = nullptr;
        pipelineLayout = nullptr;
        queue = nullptr;
        semaphoreImageAvailable = nullptr;
        semaphoreRenderingDone = nullptr;
        fenceInFlight = nullptr;
        indexBuffer = nullptr;
        indexBufferMemory = nullptr;
        vertexBuffer = nullptr;
        vertexBufferMemory = nullptr;
        descriptorSetLayout = nullptr;
        descriptorPool = nullptr;
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

    bool VulkanPrototype::checkInstanceExtensionSupport(std::vector<const char*> instanceExtensions)
    {
        uint32_t amountOfExtensions = 0;
        VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
        evaluteVulkanResult(result);

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
        
        return true;
    }

    bool VulkanPrototype::checkInstanceLayerSupport(std::vector<const char *> instanceLayers)
    {
        uint32_t amountOfLayers = 0;
        VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
        evaluteVulkanResult(result);

        std::vector<VkLayerProperties> layers;
        layers.resize(amountOfLayers);
        result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());
        evaluteVulkanResult(result);

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

        return true;
    }

    VkExtent2D VulkanPrototype::chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent =
            {
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkPresentModeKHR VulkanPrototype::choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkSurfaceFormatKHR VulkanPrototype::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    int VulkanPrototype::cleanupGlfw()
    {
        glfwDestroyWindow(window);
        return 0;
    }

    void VulkanPrototype::cleanupSwapchain()
    {
        for (uint32_t i = 0; i < framebuffers.size(); i++)
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);

        for (uint32_t i = 0; i < imageViews.size(); i++)
            vkDestroyImageView(device, imageViews[i], nullptr);

        vkDestroySwapchainKHR(device, windowData.Swapchain, nullptr);
    }

    int VulkanPrototype::cleanupVulkan()
    {
        vkDeviceWaitIdle(device);

        vkDestroySemaphore(device, semaphoreRenderingDone, pAllocator);
        vkDestroySemaphore(device, semaphoreImageAvailable, pAllocator);
        vkDestroyFence(device, fenceInFlight, pAllocator);

        cleanupSwapchain();

        vkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        vkDestroyRenderPass(device, windowData.RenderPass, pAllocator);
        vkDestroyPipeline(device, windowData.Pipeline, pAllocator);
        vkDestroyCommandPool(device, commandPool, pAllocator);

        vkDestroyDescriptorPool(device, descriptorPool, pAllocator);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);

        for (uint64_t i = 0; i < uniformBuffers.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], pAllocator);
            vkFreeMemory(device, uniformBuffersMemory[i], pAllocator);
        }
        vkDestroyBuffer(device, indexBuffer, pAllocator);
        vkFreeMemory(device, indexBufferMemory, pAllocator);
        vkDestroyBuffer(device, vertexBuffer, pAllocator);
        vkFreeMemory(device, vertexBufferMemory, pAllocator);

        vkDestroyDevice(device, pAllocator);
        vkDestroySurfaceKHR(instance, windowData.Surface, pAllocator);

#ifdef DEBUG
        auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (vkDestroyDebugUtilsMessengerEXT != nullptr) {
            vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, pAllocator);
        }
#endif

        vkDestroyInstance(instance, pAllocator);

        return 0;
    }

    void VulkanPrototype::copyBuffer(uint64_t size, VkBuffer srcBuffer, VkBuffer dstBuffer)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr
        };

        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        VkBufferCopy copyRegion =
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
        };

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void VulkanPrototype::createBuffer(uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkResult result;

        VkBufferCreateInfo bufferCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = usage,
            .sharingMode = sharingMode,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer);
        evaluteVulkanResult(result);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = pickMemoryType(memoryRequirements.memoryTypeBits, properties)
        };

        result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory);
        evaluteVulkanResult(result);

        result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createCommandBuffers(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = wd.ImageCount
        };

        commandBuffers.resize(wd.ImageCount);
        result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());
    }

    void VulkanPrototype::createCommandPool(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        //TODO: Check flags according to Vulkan Tutorial
        VkCommandPoolCreateInfo commandPoolCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamily.index.value()
        };

        result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createDescriptorPool(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        VkDescriptorPoolSize descriptorPoolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = wd.ImageCount
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = wd.ImageCount,
            .poolSizeCount = 1,
            .pPoolSizes = &descriptorPoolSize
        };

        result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, pAllocator, &descriptorPool);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createDescriptorSetLayout()
    {
        VkResult result;

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding =
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
        };

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = 1,
            .pBindings = &descriptorSetLayoutBinding
        };

        result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createDescriptorSets()
    {
        VkResult result;

        std::vector<VkDescriptorSetLayout> layouts(windowData.ImageCount, descriptorSetLayout);
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = windowData.ImageCount,
            .pSetLayouts = layouts.data()
        };

        descriptorSets.resize(windowData.ImageCount);
        result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data());
        evaluteVulkanResult(result);

        for (uint32_t i = 0; i < windowData.ImageCount; i++)
        {
            VkDescriptorBufferInfo bufferInfo =
            {
                .buffer = uniformBuffers[i],
                .offset = 0,
                .range = sizeof(UniformBufferObject)
            };

            VkWriteDescriptorSet writeDescriptorSet =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfo,
                .pTexelBufferView = nullptr
            };

            vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
        }
    }

    void VulkanPrototype::createFramebuffers(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        framebuffers.resize(wd.ImageCount);
        for (uint32_t i = 0; i < wd.ImageCount; i++)
        {
            VkFramebufferCreateInfo framebufferCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = wd.RenderPass,
                .attachmentCount = 1,
                .pAttachments = &(imageViews[i]),
                .width = static_cast<uint32_t>(wd.Width),
                .height = static_cast<uint32_t>(wd.Height),
                .layers = 1
            };

            result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(framebuffers[i]));
            evaluteVulkanResult(result);
        }
    }

    void VulkanPrototype::createGraphicsPipeline(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        std::vector<char> shaderCodeVert, shaderCodeFrag;

        try
        {
            readFile("shader/vert.spv", shaderCodeVert);
            readFile("shader/frag.spv", shaderCodeFrag);
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            evaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
        }

        VkShaderModule shaderModuleVert, shaderModuleFrag;
        createShaderModule(shaderCodeVert, &shaderModuleVert);
        createShaderModule(shaderCodeFrag, &shaderModuleFrag);

        VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderModuleVert,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
            shaderStageCreateInfoFrag =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderModuleFrag,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        //TODO: Eventuell windowData als parameter �bergeben (konsistenz)
        //TODO: Viewport is never changed on resize, needs to maybe be recreated.
        VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)wd.Width,
            .height = (float)wd.Height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor =
        {
            .offset = { 0, 0 },
            .extent = { static_cast<uint32_t>(wd.Width), static_cast<uint32_t>(wd.Height) }
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
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
            .setLayoutCount = 1,
            .pSetLayouts = &descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        result = vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &pipelineLayout);
        evaluteVulkanResult(result);

        VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputCreateInfo,
            .pInputAssemblyState = &inputAssemblyCreateInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportStateCreateInfo,
            .pRasterizationState = &rasterizationCreateInfo,
            .pMultisampleState = &multisampleCreateInfo,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendCreateInfo,
            .pDynamicState = nullptr,
            .layout = pipelineLayout,
            .renderPass = wd.RenderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &wd.Pipeline);
        evaluteVulkanResult(result);

        vkDestroyShaderModule(device, shaderModuleVert, nullptr);
        vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    }

    void VulkanPrototype::createImageViews(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        result = vkGetSwapchainImagesKHR(device, wd.Swapchain, &wd.ImageCount, nullptr);
        evaluteVulkanResult(result);
        std::vector<VkImage> swapchainImages(wd.ImageCount);
        result = vkGetSwapchainImagesKHR(device, wd.Swapchain, &wd.ImageCount, swapchainImages.data());
        evaluteVulkanResult(result);

        imageViews.resize(wd.ImageCount);

        for (uint32_t i = 0; i < wd.ImageCount; i++)
        {

            VkImageViewCreateInfo imageViewCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = swapchainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = wd.SurfaceFormat.format,
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

            result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
            evaluteVulkanResult(result);
        }
    }

    void VulkanPrototype::createIndexBuffer()
    {
        uint64_t bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        copyBuffer(bufferSize, stagingBuffer, indexBuffer);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    int VulkanPrototype::createInstance()
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

        uint32_t amountOfGlfwExtensions = 0;
        const char** requiredGlfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);
        std::vector<const char*> instanceExtensions(requiredGlfwExtensions, requiredGlfwExtensions + amountOfGlfwExtensions);

        if (!checkInstanceExtensionSupport(instanceExtensions))
        {
            std::cout << "Extension not Supported!\n";
            return -1;
        }

#ifdef DEBUG

        std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };

        if (!checkInstanceLayerSupport(instanceLayers))
        {
            std::cout << "Validation Layer not Supported!\n";
            return -1;
        }

        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!checkInstanceExtensionSupport(instanceExtensions))
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

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        evaluteVulkanResult(result);

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr
        };

        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (vkCreateDebugUtilsMessengerEXT == nullptr)
            return -1;

        result = vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
        evaluteVulkanResult(result);

#else

        VkInstanceCreateInfo instanceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
            .ppEnabledExtensionNames = instanceExtensions.data()
        };

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        evaluteVulkanResult(result);

#endif

        return 0;
    }

    void VulkanPrototype::createLogicalDevice(VkPhysicalDevice physicalDevice)
    {
        VkResult result;

        queueFamily = pickQueueFamily(physicalDevice);

        std::vector<float> queuePriorities(queueFamily.queueCount);
        for (uint32_t i = 0; i < queueFamily.queueCount; i++)
            queuePriorities[i] = 1.0f;

        VkDeviceQueueCreateInfo deviceQueueCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamily.index.value(),
            .queueCount = queueFamily.queueCount,
            .pQueuePriorities = queuePriorities.data()
        };

        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

        //TODO: Device Extensions �berpr�fen.
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

        result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        evaluteVulkanResult(result);

        vkGetDeviceQueue(device, queueFamily.index.value(), 0, &queue);
    }

    void VulkanPrototype::createRenderPass(ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        VkAttachmentDescription attachmentDescription =
        {
            .flags = 0,
            .format = wd.SurfaceFormat.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference attachmentReference =
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpassDescription =
        {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentReference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr
        };

        //TODO: Check if rendering is not done properly without this struct
        VkSubpassDependency subpassDependency =
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0
        };

        VkRenderPassCreateInfo renderPassCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescription,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 1,
            .pDependencies = &subpassDependency
        };

        result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &wd.RenderPass);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createSemaphores()
    {
        VkResult result;

        VkSemaphoreCreateInfo semaphoreCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreImageAvailable);
        evaluteVulkanResult(result);
        result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderingDone);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = shaderCode.size(),
            .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
        };

        VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, shaderModule);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createSwapchain(VkPhysicalDevice physicalDevice, ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;
        SurfaceDetails surfaceDetails = querySurfaceCapabilities(physicalDevice, windowData);

        wd.SurfaceFormat = chooseSurfaceFormat(surfaceDetails.formats);
        VkPresentModeKHR presentMode = choosePresentMode(surfaceDetails.presentModes);
        VkExtent2D swapchainExtent = chooseExtent2D(surfaceDetails.capabilities);

        wd.Width = swapchainExtent.width;
        wd.Height = swapchainExtent.height;

        uint32_t imageCount = surfaceDetails.capabilities.minImageCount + 1;

        if (surfaceDetails.capabilities.maxImageCount > 0 && imageCount > surfaceDetails.capabilities.maxImageCount)
        {
            imageCount = surfaceDetails.capabilities.maxImageCount;
        }

        //TODO: Parameter �berpr�fen
        VkSwapchainCreateInfoKHR swapchainCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = wd.Surface,
            .minImageCount = imageCount,
            .imageFormat = wd.SurfaceFormat.format,
            .imageColorSpace = wd.SurfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = surfaceDetails.capabilities.currentTransform,//VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };

        result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &wd.Swapchain);
        evaluteVulkanResult(result);
    }

    void VulkanPrototype::createUniformBuffers(ImGui_ImplVulkanH_Window& wd)
    {
        uint64_t bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(wd.ImageCount);
        uniformBuffersMemory.resize(wd.ImageCount);

        for (uint64_t i = 0; i < wd.ImageCount; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    void VulkanPrototype::createVertexBuffer()
    {
        VkResult result;

        uint64_t bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        copyBuffer(bufferSize, stagingBuffer, vertexBuffer);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void VulkanPrototype::drawFrame()
    {
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, windowData.Swapchain, UINT64_MAX, semaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex);
        evaluteVulkanResult(result);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain();
            return;
        }

        updateUniformBuffer(imageIndex, windowData);

        VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphoreImageAvailable,
            .pWaitDstStageMask = waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &(commandBuffers[imageIndex]),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &semaphoreRenderingDone
        };

        //TODO: Check if a fence might be neccessary for higher load
        result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        evaluteVulkanResult(result);

        VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphoreRenderingDone,
            .swapchainCount = 1,
            .pSwapchains = &windowData.Swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr
        };

        result = vkQueuePresentKHR(queue, &presentInfo);
        evaluteVulkanResult(result);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            recreateSwapchain();
        }
    }

    int VulkanPrototype::initializeGlfw()
    {
        if (!glfwInit())
        {
            std::cerr << "Could not initalize GLFW!\n";
            return -1;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        if (!glfwVulkanSupported())
        {
            std::cerr << "GLFW: Vulkan not supported!\n";
            return -1;
        }

        window = glfwCreateWindow(windowData.Width, windowData.Height, "VulkanPrototype", nullptr, nullptr);

        return 0;
    }

    int VulkanPrototype::initializeVulkan()
    {
        VkResult result;

        if (createInstance() != 0)
            return -1;

        result = glfwCreateWindowSurface(instance, window, nullptr, &windowData.Surface);
        evaluteVulkanResult(result);

        physicalDevice = pickPhysicalDevice();

        createLogicalDevice(physicalDevice);

        VkBool32 surfaceSupport = false;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamily.index.value(), windowData.Surface, &surfaceSupport);
        evaluteVulkanResult(result);

        if (!surfaceSupport)
        {
            std::cout << "Surface not Supported";
            evaluteVulkanResult(VK_ERROR_INITIALIZATION_FAILED);
            return -1;
        }

        //TODO: replace createSwapchain(), createImageViews(), createRenderPass() and createGraphicsPipeline() with ImGui impl
        createSwapchain(physicalDevice, windowData);
        createImageViews(windowData);
        createRenderPass(windowData);
        createDescriptorSetLayout();

        createGraphicsPipeline(windowData);

        createFramebuffers(windowData);
        createCommandPool(windowData);

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers(windowData);

        createDescriptorPool(windowData);
        createDescriptorSets();

        createCommandBuffers(windowData);
        recordCommandBuffers(commandBuffers, framebuffers, windowData);

        createSemaphores();

        VkFenceCreateInfo fenceCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        return 0;
    }

    int VulkanPrototype::mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            glfwPollEvents();
            drawFrame();

            //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            //std::cout << 1000000 / std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[fps]\n";
        }

        return 0;
    }

    uint32_t VulkanPrototype::pickMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

        for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkPhysicalDevice VulkanPrototype::pickPhysicalDevice()
    {
        uint32_t amountOfPhysicalDevices = 0;
        VkResult result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
        evaluteVulkanResult(result);

        std::vector<VkPhysicalDevice> physicalDevices(amountOfPhysicalDevices);
        result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
        evaluteVulkanResult(result);

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

    QueueFamily VulkanPrototype::pickQueueFamily(VkPhysicalDevice physicalDevice)
    {
        QueueFamily queueFamily;

        uint32_t amountOfQueueFamilyProperties = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(amountOfQueueFamilyProperties);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfQueueFamilyProperties, queueFamilyProperties.data());

        for (uint32_t i = 0; i < amountOfQueueFamilyProperties; i++)
        {
            if (queueFamilyProperties[i].queueFlags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
            {
                queueFamily.index = i;
                queueFamily.queueCount = 1;

                break;
            }
        }

        if (!queueFamily.index.has_value())
            throw std::logic_error("No fitting QueueFamily was found."); //TODO: Inconsisten throw

        return queueFamily;
    }

    SurfaceDetails VulkanPrototype::querySurfaceCapabilities(VkPhysicalDevice physicalDevice, ImGui_ImplVulkanH_Window& wd)
    {
        SurfaceDetails surfaceDetails;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, wd.Surface, &surfaceDetails.capabilities);

        uint32_t amountOfSurfaceFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, wd.Surface, &amountOfSurfaceFormats, nullptr);
        surfaceDetails.formats.resize(amountOfSurfaceFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, wd.Surface, &amountOfSurfaceFormats, surfaceDetails.formats.data());

        uint32_t amountOFPresentModes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, wd.Surface, &amountOFPresentModes, nullptr);
        surfaceDetails.presentModes.resize(amountOFPresentModes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, wd.Surface, &amountOFPresentModes, surfaceDetails.presentModes.data());

        return surfaceDetails;
    }

    void VulkanPrototype::readFile(const std::string& filename, std::vector<char>& buffer)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (file.is_open())
        {
            size_t fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0);
            buffer.resize(fileSize);
            file.read(buffer.data(), fileSize);
            file.close();
        }
        else
        {
            throw std::runtime_error("Datei \"" + filename + "\" konnte nicht ge�ffnet werden!");
        }
    }

    void VulkanPrototype::recreateSwapchain()
    {
        vkDeviceWaitIdle(device);

        cleanupSwapchain();

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        createSwapchain(physicalDevice, windowData);
        createImageViews(windowData);
        createFramebuffers(windowData);

        recordCommandBuffers(commandBuffers, framebuffers, windowData);
    }

    void VulkanPrototype::recordCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, std::vector<VkFramebuffer>& framebuffers, ImGui_ImplVulkanH_Window& wd)
    {
        VkResult result;

        VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = nullptr
        };

        for (uint32_t i = 0; i < wd.ImageCount; i++)
        {
            result = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);
            evaluteVulkanResult(result);

            VkClearValue clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

            VkRenderPassBeginInfo renderPassBeginInfo =
            {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = wd.RenderPass,
                .framebuffer = framebuffers[i],
                .renderArea = {{0, 0}, {static_cast<uint32_t>(wd.Width), static_cast<uint32_t>(wd.Height)}},
                .clearValueCount = 1,
                .pClearValues = &clearValue
            };

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, wd.Pipeline);

            VkBuffer vertexBuffers[] = { vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

            result = vkEndCommandBuffer(commandBuffers[i]);
            evaluteVulkanResult(result);
        }
    }

    void VulkanPrototype::updateUniformBuffer(uint32_t imageIndex, ImGui_ImplVulkanH_Window& wd)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo =
        {
            .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            .view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            .proj = glm::perspective(glm::radians(60.0f), (float)wd.Width / (float)wd.Height, 0.1f, 10.0f)
        };

        ubo.proj[1][1] *= -1;

        void* data;
        vkMapMemory(device, uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniformBuffersMemory[imageIndex]);
    }
}