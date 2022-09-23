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

if os.host() == "windows" then
    include "vendor/premake5_glfw.lua"
    os.execute("cd VulkanPrototype/shader/ && runShaderCompiler.bat")
end

if os.host() == "linux" then
    os.execute("cd VulkanPrototype/shader/ && bash runShaderCompiler.sh")
end