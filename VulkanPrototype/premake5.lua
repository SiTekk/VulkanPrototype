project "VulkanPrototype"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir ("../out/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../out/obj/" .. outputdir .. "/%{prj.name}")

    files {
        "src/*.h",
        "src/*.cpp"
    }

    includedirs {
        "../vendor/imgui",
        "../vendor/imgui/backends",
        "../vendor/glfw/include",
        "../vendor/glm",

        "%{VULKAN_INCLUDE}"
    }

    links {
        "ImGui",
        "GLFW",

        "%{VULKAN_LIB}"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"