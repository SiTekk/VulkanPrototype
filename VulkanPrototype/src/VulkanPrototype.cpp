#include "VulkanPrototype.h"

#include "Backend/Backend.h"
#include "Renderer/Renderer.h"

namespace VulkanPrototype
{
    int Run()
    {
        Backend::Initialize(Renderer::windowSize.width, Renderer::windowSize.height);
        Renderer::initializeVulkan();
        Renderer::initializeImGui();
        mainLoop();
        Renderer::cleanupImGui();
        Renderer::cleanupVulkan();
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
            ImGui::SliderFloat("Angle", &Renderer::uboValues.angle, 0.0f, 360.0f, "%.2f", ImGuiSliderFlags_ClampOnInput);
            ImGui::SliderFloat("Axis x", &Renderer::uboValues.axis.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Axis y", &Renderer::uboValues.axis.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Axis z", &Renderer::uboValues.axis.z, -1.0f, 1.0f);

            ImGui::Text("View:");
            ImGui::SliderFloat("Eye x", &Renderer::uboValues.eye.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Eye y", &Renderer::uboValues.eye.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Eye z", &Renderer::uboValues.eye.z, -1.0f, 1.0f);
            ImGui::SliderFloat("Center x", &Renderer::uboValues.center.x, -5.0f, 5.0f);
            ImGui::SliderFloat("Center y", &Renderer::uboValues.center.y, -5.0f, 5.0f);
            ImGui::SliderFloat("Center z", &Renderer::uboValues.center.z, -5.0f, 5.0f);

            ImGui::Text("Project:");
            ImGui::SliderFloat("Fovy", &Renderer::uboValues.fovy, 0.0f, 360.0f);
            ImGui::SliderFloat("Near", &Renderer::uboValues.near, 0.0f, 20.0f);
            ImGui::SliderFloat("Far", &Renderer::uboValues.far, 0.0f, 20.0f);

            ImGui::End();

            ImGui::ShowDemoWindow(nullptr);

            //Render Data and record Command Buffers
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();

            Renderer::frameRender(draw_data);
        }

        //vkDeviceWaitIdle(device);

        return 0;
    }
}