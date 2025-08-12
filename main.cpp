#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include <vector>
#include "Objects.h"
#include "Scene.h"
#include "ColorPicker.h"
#include "constants.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Scene &scene, ColorPicker& colorPicker, GizmoState& gizmo);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


glm::vec3 getMouseWorldPosition(GLFWwindow* window);

// Scene object
Scene scene;

// basic cube vertices, can be scaled later

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Gizmo State Manager
GizmoState gizmo;

// Global pointer to color picker object
ColorPicker* colorPickPoint;


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // tell GLFW to capture our mouse
    //bool rightClickHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    //static bool wasHoldingRightClick = false;

    //if (rightClickHeld && !wasHoldingRightClick) {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //    firstMouse = true; // reset for smooth movement when entering camera mode
    //}
    //else if (!rightClickHeld && wasHoldingRightClick) {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //}

    //wasHoldingRightClick = rightClickHeld;

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // setup ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Or ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");  // Your GLSL version



    // build and compile shaders
    // -------------------------
    Shader ourShader("Vertex.vs", "Fragment.fs");

    // Color picker FBO
    Shader colorPickShader("Vertex.vs", "ColorPickerFrag.fs");
    ColorPicker colorPicker(scene, colorPickShader, camera);
    colorPickPoint = &colorPicker; // for scope purposes

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // input
        // -----
        processInput(window, scene, colorPicker, gizmo);

        // Render picking pass
        colorPicker.renderPickingPass();


        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw your ImGui GUI
        ImGui::SetNextWindowSize(ImVec2(200, 200)); // width = 400, height = 300
        ImGui::Begin("My Window");
        ImGui::Text("Hello from ImGui!");
        if (ImGui::Button("Cube")) {
            scene.addObj(new Cube());
        }
        ImGui::End();

        scene.draw(ourShader);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //close ImGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, Scene& scene, ColorPicker& colorPicker, GizmoState& gizmo)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static bool leftMousePressedLastFrame = false;
    ImGuiIO& io = ImGui::GetIO();
    bool leftMousePressedNow = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftMousePressedNow && leftMousePressedLastFrame && !io.WantCaptureMouse) {
        if (gizmo.isMoving) {
            glm::vec3 currentMousePos = getMouseWorldPosition(window);
            glm::vec3 delta = currentMousePos - gizmo.initialClickPos;

            switch (gizmo.ActiveAxis) {
            case MoveAxis::X:
                scene.getSelectedObj()->position.x += delta.x;
                break;
            case MoveAxis::Y:
                scene.getSelectedObj()->position.y += delta.y;
                break;
            case MoveAxis::Z:
                scene.getSelectedObj()->position.z += delta.z;
                break;
            default:
                break;
            }

            gizmo.initialClickPos = currentMousePos;
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !io.WantCaptureMouse) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    leftMousePressedLastFrame = leftMousePressedNow;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !io.WantCaptureMouse) {
        // Get cursor position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int winWidth, winHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);

        int id = colorPickPoint->getObjectIDAtPixel((int)mouseX, (int)mouseY, winHeight);

        if (id != -1) {
            if (id == GIZMO_RED_ID) {
                Object* sel = scene.getSelectedObj();
                if (sel != nullptr) {
                    gizmo.isMoving = true;
                    gizmo.ActiveAxis = MoveAxis::Z;
                    gizmo.initialClickPos = getMouseWorldPosition(window);
                }
            }
            else if (id == GIZMO_GREEN_ID) {
                Object* sel = scene.getSelectedObj();
                if (sel != nullptr) {
                    std::cout << "GREEN" << std::endl;
                    gizmo.isMoving = true;
                    gizmo.ActiveAxis = MoveAxis::X;
                    gizmo.initialClickPos = getMouseWorldPosition(window);
                }
            }
            else if (id == GIZMO_BLUE_ID) {
                Object* sel = scene.getSelectedObj();
                if (sel != nullptr) {
                    gizmo.isMoving = true;
                    gizmo.ActiveAxis = MoveAxis::Y;
                    gizmo.initialClickPos = getMouseWorldPosition(window);
                }
            }
            else {
                int index = id - 1;
                Object* obj = scene.getObjs()[index];
                if (!obj->isSelected()) {
                    obj->toggleSelected();
                    scene.selectObject(obj);
                }
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !io.WantCaptureMouse) {
        gizmo.isMoving = false;
        gizmo.ActiveAxis = MoveAxis::None;
    }
}



// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

glm::vec3 getMouseWorldPosition(GLFWwindow* window)
{
    double mouseX_d, mouseY_d;
    glfwGetCursorPos(window, &mouseX_d, &mouseY_d);
    float mouseX = static_cast<float>(mouseX_d);
    float mouseY = static_cast<float>(mouseY_d);

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    // flip Y for OpenGL window coordinates
    float readY = (float)(winHeight - 1) - mouseY;

    // Read depth from the depth buffer (call after rendering, before swap)
    float depth = 1.0f;
    glFlush(); // ensure commands are flushed to GPU before read
    glReadPixels((int)mouseX, (int)readY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    if (depth >= 1.0f) {
        // clicked background/sky — return a point on near plane as fallback or signal no hit
        // returning camera position + forward*near is often useful:
        return camera.Position + camera.Front * 0.1f;
    }

    glm::vec3 ndc;
    ndc.x = (2.0f * mouseX) / winWidth - 1.0f;
    ndc.y = 1.0f - (2.0f * mouseY) / winHeight;
    ndc.z = depth * 2.0f - 1.0f; // convert [0,1] depth to NDC z

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
        (float)winWidth / (float)winHeight,
        0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::vec4 viewport = glm::vec4(0, 0, winWidth, winHeight);

    glm::vec3 screenPos(mouseX, readY, depth);
    glm::vec3 worldPos = glm::unProject(screenPos, view, projection, viewport);
    return worldPos;
}
