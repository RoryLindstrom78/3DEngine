#pragma once

#include "scene.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class UIManager {
public:
	UIManager(Scene* scene) : scene(scene) {}

    void draw() {

        drawObjectCreationWindow();

        if (scene->getSelectedObj()) {
            drawMaterialWindow(scene->getSelectedObj());
        }
    }

private:
	Scene* scene;

    void drawObjectCreationWindow() {
        // Draw your ImGui GUI
        ImGui::SetNextWindowSize(ImVec2(200, 200)); // width = 400, height = 300
        ImGui::Begin("My Window");
        ImGui::Text("Hello from ImGui!");
        if (ImGui::Button("Cube")) {
            scene->addObj(new Cube());
        }
        if (ImGui::Button("Sphere")) {
            scene->addObj(new Sphere());
        }

        ImGui::End();
    }

    void drawMaterialWindow(Object* obj) {
        ImGui::SetNextWindowSize(ImVec2(400, 100));
        ImGui::Begin("Material Properties");
        ImGui::SliderFloat("Metallic", &obj->metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &obj->roughness, 0.05f, 1.0f);
        ImGui::ColorEdit3("Color", (float*)&obj->color); // or ColorPicker3
        ImGui::End();
    }
};