project "VulkanPrototype"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    warnings "Extra"
    targetdir ("../out/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../out/obj/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    filter "system:windows"
        includedirs {
            "../vendor/glfw/include",
            "../vendor/glm",
            "../vendor/imgui",
            "../vendor/imgui/backends",
            "../vendor/stb",
            "../vendor/VulkanMemoryAllocator/include",

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


    filter "system:linux"
        includedirs {
            "../vendor/imgui",
            "../vendor/imgui/backends",
            "../vendor/glm",
            "../vendor/stb"
        }

        links {
            "ImGui",
            "glfw",
            "vulkan",
            "dl",
            "pthread",
            "X11",
            "Xxf86vm",
            "Xrandr",
            "Xi"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"