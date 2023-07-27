#include "VulkanPrototype.h"

#include "Backend/Backend.h"
#include "Renderer/Renderer.h"

namespace VulkanPrototype
{
    void handleInputs(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            Renderer::g_uboValues.eye.z += 0.1;
            Renderer::g_uboValues.center.z += 0.1;
        }
        else if (glfwGetKey(window, GLFW_KEY_A))
        {
            Renderer::g_uboValues.eye.x -= 0.1;
            Renderer::g_uboValues.center.x -= 0.1;
        }
        else if (glfwGetKey(window, GLFW_KEY_S))
        {
            Renderer::g_uboValues.eye.z -= 0.1;
            Renderer::g_uboValues.center.z -= 0.1;
        }
        else if (glfwGetKey(window, GLFW_KEY_D))
        {
            Renderer::g_uboValues.eye.x += 0.1;
            Renderer::g_uboValues.center.x += 0.1;
        }

        static double old_xpos = 0, old_ypos = 0;
        double xpos = 0, ypos = 0;

        glfwGetCursorPos(window, &xpos, &ypos);
        Renderer::g_uboValues.center.x += sin(0.01 * (xpos - old_xpos));
        Renderer::g_uboValues.center.y += sin(0.01 * (ypos - old_ypos));

        old_xpos = xpos;
        old_ypos = ypos;
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_W && action)
        {
            Renderer::g_uboValues.eye.z += 0.1;
            Renderer::g_uboValues.center.z += 0.1;
        }
        else if (key == GLFW_KEY_A && action)
        {
            Renderer::g_uboValues.eye.x -= 0.1;
            Renderer::g_uboValues.center.x -= 0.1;
        }
        else if (key == GLFW_KEY_S && action)
        {
            Renderer::g_uboValues.eye.z -= 0.1;
            Renderer::g_uboValues.center.z -= 0.1;
        }
        else if (key == GLFW_KEY_D && action)
        {
            Renderer::g_uboValues.eye.x += 0.1;
            Renderer::g_uboValues.center.x += 0.1;
        }
    }

    int mainLoop()
    {
        //glfwSetKeyCallback(Backend::g_window, key_callback);
        glfwSetInputMode(Backend::g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        while (!glfwWindowShouldClose(Backend::g_window))
        {
            //GlfwEvents
            glfwPollEvents();
            handleInputs(Backend::g_window);

            //Setup ImGui
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Game Logic
            ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_NoTitleBar);

            ImGui::Text("Settings for Unifor Buffer Object:");

            ImGui::Text("Model:");
            ImGui::SliderFloat("Angle", &Renderer::g_uboValues.angle, -360.0f, 360.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
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

            static bool check = false;
            if (ImGui::Checkbox("Enable Polygon Mode Line", &check))
            {
                Renderer::g_polygonMode = check ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
            }

            ImGui::End();

            //ImGui::ShowDemoWindow(nullptr);

            //Render Data and record Command Buffers
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();

            Renderer::RenderFrame(draw_data);
        }

        //vkDeviceWaitIdle(device);

        return 0;
    }

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
}