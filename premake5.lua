workspace "VulkanPrototype"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "VulkanPrototype"

outputdir = "%{cfg.buildcfg}"

VULKAN_SDK = os.getenv("VULKAN_SDK")
VULKAN_INCLUDE = "%{VULKAN_SDK}/Include"
VULKAN_LIB = "%{VULKAN_SDK}/Lib/vulkan-1.lib"

include "VulkanPrototype"
include "vendor/premake5_imgui.lua"
