project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "off"

    targetdir ("../out/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../out/obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "imgui/imconfig.h",
        "imgui/imgui.h",
        "imgui/imgui.cpp",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_internal.h",
        "imgui/imgui_tables.cpp",
        "imgui/imgui_widgets.cpp",
        "imgui/imstb_rectpack.h",
        "imgui/imstb_textedit.h",
        "imgui/imstb_truetype.h",
    }

    includedirs {
        "../vendor/imgui",
        "../vendor/imgui/backends",
        "../vendor/glfw/include",

        "%{VULKAN_INCLUDE}"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"

    filter "system:linux"
        pic "On"
        systemversion "latest"
        cppdialect "C++17"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"