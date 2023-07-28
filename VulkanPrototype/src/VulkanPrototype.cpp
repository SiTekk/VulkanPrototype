#include "VulkanPrototype.h"

#include <chrono>

#include "Backend/Backend.h"
#include "Renderer/Renderer.h"

namespace VulkanPrototype
{
    void handleInputs(GLFWwindow* window)
    {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = currentTime - lastTime;

        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            Renderer::g_uboValues.eye += deltaTime.count() * 0.00000001f * Renderer::g_uboValues.center;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            Renderer::g_uboValues.eye -= deltaTime.count() * 0.00000001f * (glm::normalize(glm::cross( Renderer::g_uboValues.center, Renderer::g_uboValues.up)));
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            Renderer::g_uboValues.eye -= deltaTime.count() * 0.00000001f * Renderer::g_uboValues.center;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            Renderer::g_uboValues.eye += deltaTime.count() * 0.00000001f * (glm::normalize(glm::cross(Renderer::g_uboValues.center, Renderer::g_uboValues.up)));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            Renderer::g_uboValues.eye -= deltaTime.count() * 0.00000001f * (glm::normalize(glm::cross(Renderer::g_uboValues.center, glm::cross(Renderer::g_uboValues.center, Renderer::g_uboValues.up))));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
        {
            Renderer::g_uboValues.eye += deltaTime.count() * 0.00000001f * (glm::normalize(glm::cross(Renderer::g_uboValues.center, glm::cross(Renderer::g_uboValues.center, Renderer::g_uboValues.up))));
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        static int mouseMode = glfwGetInputMode(window, GLFW_CURSOR);

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            mouseMode ^= 2;
            glfwSetInputMode(window, GLFW_CURSOR, mouseMode);
        }
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        static const float sensititvity = 0.1f;
        static float old_xpos = 0, old_ypos = 0;
        static float yaw = 0, pitch = 0;

        float delta_x = (old_xpos - static_cast<float>(xpos)) * sensititvity;
        float delta_y = (old_ypos - static_cast<float>(ypos)) * sensititvity;

        old_xpos = static_cast<float>(xpos);
        old_ypos = static_cast<float>(ypos);

        if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
            return;

        yaw += delta_x;
        pitch += delta_y;

        if (pitch > 89.0f)
            pitch = 89.0f;
        else if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction(0);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = -1.0f * sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        Renderer::g_uboValues.center = glm::normalize(direction);
    }

    int mainLoop()
    {
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

        //TODO: Put Callbacks in specific function
        //Needs to be before ImguiInit !!!
        glfwSetKeyCallback(Backend::g_window, key_callback);
        glfwSetCursorPosCallback(Backend::g_window, mouse_callback);

        if (Renderer::Initialize())
            return 0;

        mainLoop();

        Renderer::Cleanup();
        Backend::Cleanup();

        return 0;
    }
}