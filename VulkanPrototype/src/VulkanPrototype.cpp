#include "VulkanPrototype.h"

#include "Backend/Backend.h"
#include "Renderer/Renderer.h"

namespace VulkanPrototype
{
    int Run()
    {
        if (Backend::Initialize(Renderer::g_windowSize.width, Renderer::g_windowSize.height))
            return 0;
        if (Renderer::Initialize())
            return 0;

        mainLoop();

        Renderer::Cleanup();
        Backend::Cleanup();

        return 0;
    }

    int mainLoop()
    {
        while (!glfwWindowShouldClose(Backend::g_window))
        {
            //GlfwEvents
            glfwPollEvents();

            //Setup ImGui
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Game Logic
            ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_NoTitleBar);

            ImGui::Text("Settings for Unifor Buffer Object:");

            ImGui::Text("Model:");
            ImGui::SliderFloat("Angle", &Renderer::g_uboValues.angle, 0.0f, 360.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Axis x", &Renderer::g_uboValues.axis.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Axis y", &Renderer::g_uboValues.axis.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Axis z", &Renderer::g_uboValues.axis.z, -1.0f, 1.0f);

            ImGui::Text("View:");
            ImGui::SliderFloat("Eye x", &Renderer::g_uboValues.eye.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Eye y", &Renderer::g_uboValues.eye.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Eye z", &Renderer::g_uboValues.eye.z, -1.0f, 1.0f);
            ImGui::SliderFloat("Center x", &Renderer::g_uboValues.center.x, -5.0f, 5.0f);
            ImGui::SliderFloat("Center y", &Renderer::g_uboValues.center.y, -5.0f, 5.0f);
            ImGui::SliderFloat("Center z", &Renderer::g_uboValues.center.z, -5.0f, 5.0f);

            ImGui::Text("Project:");
            ImGui::SliderFloat("Fovy", &Renderer::g_uboValues.fovy, 0.0f, 360.0f);
            ImGui::SliderFloat("Near", &Renderer::g_uboValues.near, 0.0f, 20.0f);
            ImGui::SliderFloat("Far", &Renderer::g_uboValues.far, 0.0f, 20.0f);

            ImGui::End();

            ImGui::ShowDemoWindow(nullptr);

            //Render Data and record Command Buffers
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();

            Renderer::RenderFrame(draw_data);
        }

        //vkDeviceWaitIdle(device);

        return 0;
    }
}