#pragma once
#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#include "shader.h"
#include "stateManager.h"

// Ray intersection function
bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float& t) // output: distance along ray to intersection
{
    float tmin = (boxMin.x - rayOrigin.x) / rayDir.x;
    float tmax = (boxMax.x - rayOrigin.x) / rayDir.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (boxMin.y - rayOrigin.y) / rayDir.y;
    float tymax = (boxMax.y - rayOrigin.y) / rayDir.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (boxMin.z - rayOrigin.z) / rayDir.z;
    float tzmax = (boxMax.z - rayOrigin.z) / rayDir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    t = tmin;

    if (t < 0) {
        t = tmax;
        if (t < 0)
            return false;
    }

    return true;
}

// normal vectors
static float cubeNormals[] = {
    // front normal (z = +0.5)
    0.0f, 0.0f,  0.5f,  0.0f, 0.0f,  1.0f,
    // back normal (z = -0.5)
    0.0f, 0.0f, -0.5f,  0.0f, 0.0f, -1.0f,
    // right normal (x = +0.5)
    0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,
    // left normal (x = -0.5)
   -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,
   // top normal (y = +0.5)
   0.0f, 0.5f,  0.0f,  0.0f, 1.0f,  0.0f,
   // bottom normal (y = -0.5)
   0.0f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f
};

// rotation gizmo calculations
const int segments = 64;
const float radius = 0.75f;

// Segments to draw circle for rotation gizmo
std::vector<float> rotationVertices;

// Cube vertex data
float cubeVertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

float cubeEdges[] = { // edges for drawing border
    // Bottom square
    -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,

    // Top square
    -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,

    // Vertical lines
    -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
};

// Circle stuff
std::vector<float> data;
std::vector<unsigned int> indices;


// Base object class
class Object {
public:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;
    glm::quat orientation;
    float metallic;
    float roughness;
    glm::vec3 color;
    bool selected;

    int ID;

    Object(glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 sze = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        glm::quat orientation = glm::quat(glm::vec3(0.0f)),
        float metallic = 0.5, float roughness = 0.5,
        glm::vec3 color = glm::vec3(0.5, 0.5, 0.5))
        : position(pos), size(sze), rotation(rot), orientation(orientation), metallic(metallic), roughness(roughness), color(color), selected(false) {

        // draw rotation gizmo segments here
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = radius * cos(theta);
            float y = radius * sin(theta);
            rotationVertices.push_back(x);
            rotationVertices.push_back(y);
            rotationVertices.push_back(0.0f); // z=0, so it's in XY plane
        }

    }

    virtual ~Object() = default;
    virtual void draw(Shader& shader, StateManager& state) const = 0;
    virtual void backDraw(Shader& shader, glm::vec3 color, StateManager& state) const = 0;
    virtual bool intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& distance) const = 0;
    bool isSelected() const { return selected; }
    void onSelected() { selected = true; }
    void offSelected() { selected = false; }
};

// Cube class with shared VAO/VBO
class Cube : public Object {
public:

    Cube(glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 sze = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        glm::quat orientation = glm::quat(glm::vec3(0.0f)),
        float metallic = 0.5, float roughness = 0.5,
        glm::vec3 color = glm::vec3(0.5, 0.5, 0.5))
        : Object(pos, sze, rot, orientation, metallic, roughness, color) {
        initSharedBuffers();
    }

    void draw(Shader& shader, StateManager& state) const {
        shader.use();
        shader.setBool("usePBR", true);

        shader.setFloat("metallic", metallic);
        shader.setFloat("roughness", roughness);
        shader.setVec3("albedo", color);

        // --- Draw filled cube ---
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position) *
        glm::mat4_cast(orientation) * // NEW
        glm::scale(model, size);
        shader.setMat4("model", model);
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //shader.setVec3("inColor", glm::vec3(0.9f, 0.3f, 0.3f));
         


        glBindVertexArray(sharedVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // draw border if selected
        if (selected) {
            shader.setBool("usePBR", false);
            shader.setVec3("inColor", glm::vec3(0.47f, 0.87f, 0.9f));
            shader.setMat4("model", model);
            
            glLineWidth(4.0f);

            glBindVertexArray(edgeVAO);
            glDrawArrays(GL_LINES, 0, 24);
            glBindVertexArray(0);

            if (state.getActiveTool() == GizmoTool::move) {
                // draw transform lines
                shader.setMat4("model", model); // or your transform
                glLineWidth(20.0f);
                glBindVertexArray(normalVAO);

                // Set color red for X axis lines
                shader.setVec3("inColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINES, 0, 4);

                // Set color green for Y axis lines
                shader.setVec3("inColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINES, 4, 4);

                // Set color blue for Z axis lines
                shader.setVec3("inColor", glm::vec3(0.0f, 0.0f, 1.0f));
                glDrawArrays(GL_LINES, 8, 4);
            }

            if (state.getActiveTool() == GizmoTool::rotate) {
                glLineWidth(7.5f);

                glBindVertexArray(rotVAO);
                shader.setVec3("inColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelXZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
                shader.setMat4("model", modelXZ);
                shader.setVec3("inColor", glm::vec3(0.0f, 0.0, 1.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelYZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
                shader.setMat4("model", modelYZ);
                shader.setVec3("inColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);
            }

            glBindVertexArray(0);
        }
    }

    void backDraw(Shader& shader, glm::vec3 color, StateManager& state) const override {
        shader.use();

        // --- Draw filled cube ---
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position) *
        glm::mat4_cast(orientation) * // NEW
        glm::scale(model, size);
        shader.setMat4("model", model);
        shader.setVec3("pickingColor", color);

        glBindVertexArray(sharedVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        if (selected) {
            glLineWidth(4.0f);

            if (state.getActiveTool() == GizmoTool::move) {
                // draw transform lines
                shader.setMat4("model", model); // or your transform
                glLineWidth(20.0f);
                glBindVertexArray(normalVAO);

                // Set color red for X axis lines
                shader.setVec3("pickingColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINES, 0, 4);

                // Set color green for Y axis lines
                shader.setVec3("pickingColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINES, 4, 4);

                // Set color blue for Z axis lines
                shader.setVec3("pickingColor", glm::vec3(0.0f, 0.0f, 1.0f));
                glDrawArrays(GL_LINES, 8, 4);

                glBindVertexArray(0);
            }

            if (state.getActiveTool() == GizmoTool::rotate) {
                glLineWidth(7.5f);

                glBindVertexArray(rotVAO);
                shader.setVec3("pickingColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelXZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
                shader.setMat4("model", modelXZ);
                shader.setVec3("pickingColor", glm::vec3(0.0f, 0.0, 1.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelYZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
                shader.setMat4("model", modelYZ);
                shader.setVec3("pickingColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);
            }
        }
    }

    static void cleanupSharedBuffers() {
        if (initialized) {
            glDeleteVertexArrays(1, &sharedVAO);
            glDeleteBuffers(1, &sharedVBO);
            initialized = false;
        }
    }

    bool intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& distance) const override {
        glm::vec3 boxMin = position - size * 0.5f;
        glm::vec3 boxMax = position + size * 0.5f;
        return rayIntersectsAABB(rayOrigin, rayDir, boxMin, boxMax, distance);
    }

private:
    static GLuint edgeVAO;
    static GLuint edgeVBO;
    static GLuint normalVAO;
    static GLuint normalVBO;
    static GLuint rotVAO;
    static GLuint rotVBO;
    static unsigned int sharedVAO;
    static unsigned int sharedVBO;
    static bool initialized;

    static void initSharedBuffers() {
        if (initialized) return;

        // initialize cube VAO and VBO
        glGenVertexArrays(1, &sharedVAO);
        glGenBuffers(1, &sharedVBO);

        unsigned int stride = (3 + 2 + 3) * sizeof(float);

        glBindVertexArray(sharedVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        


        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // initialize cube edge VAO and VBO
        glGenVertexArrays(1, &edgeVAO);
        glGenBuffers(1, &edgeVBO);

        glBindVertexArray(edgeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeEdges), cubeEdges, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);


        // Normal VAO and VBO
        glGenVertexArrays(1, &normalVAO);
        glGenBuffers(1, &normalVBO);

        glBindVertexArray(normalVAO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);

        // Rotation gizmo VAO and VBO
        glGenVertexArrays(1, &rotVAO);
        glGenBuffers(1, &rotVBO);

        glBindVertexArray(rotVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rotVBO);
        glBufferData(GL_ARRAY_BUFFER, rotationVertices.size() * sizeof(float), rotationVertices.data(), GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        initialized = true;
    }
};

class Sphere : public Object {
public:

    Sphere(glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 sze = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        glm::quat orientation = glm::quat(glm::vec3(0.0f)),
        float metallic = 0.5, float roughness = 0.5,
        glm::vec3 color = glm::vec3(0.5, 0.5, 0.5))
        : Object(pos, sze, rot, orientation, metallic, roughness, color) {
        initSharedBuffers();
    }

    void draw(Shader& shader, StateManager& state) const {

        shader.use();
        shader.setBool("usePBR", true);
        shader.setFloat("metallic", metallic);
        shader.setFloat("roughness", roughness);
        shader.setVec3("albedo", color);

        // Draw sphere
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position) *
            glm::mat4_cast(orientation) * // NEW
            glm::scale(model, size);
        shader.setMat4("model", model);
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //shader.setVec3("inColor", glm::vec3(0.9f, 0.3f, 0.3f));



        glBindVertexArray(sharedVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // draw border if selected
        if (selected) {
            shader.setBool("usePBR", false);
            shader.setVec3("inColor", glm::vec3(0.47f, 0.87f, 0.9f));
            shader.setMat4("model", model);

            glLineWidth(4.0f);

            glBindVertexArray(edgeVAO);
            glDrawArrays(GL_LINES, 0, 24);
            glBindVertexArray(0);

            if (state.getActiveTool() == GizmoTool::move) {
                // draw transform lines
                shader.setMat4("model", model); // or your transform
                glLineWidth(20.0f);
                glBindVertexArray(normalVAO);

                // Set color red for X axis lines
                shader.setVec3("inColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINES, 0, 4);

                // Set color green for Y axis lines
                shader.setVec3("inColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINES, 4, 4);

                // Set color blue for Z axis lines
                shader.setVec3("inColor", glm::vec3(0.0f, 0.0f, 1.0f));
                glDrawArrays(GL_LINES, 8, 4);
            }

            if (state.getActiveTool() == GizmoTool::rotate) {
                glLineWidth(7.5f);

                glBindVertexArray(rotVAO);
                shader.setVec3("inColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelXZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
                shader.setMat4("model", modelXZ);
                shader.setVec3("inColor", glm::vec3(0.0f, 0.0, 1.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelYZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
                shader.setMat4("model", modelYZ);
                shader.setVec3("inColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);
            }

            glBindVertexArray(0);
        }
    }

    void backDraw(Shader& shader, glm::vec3 color, StateManager& state) const override {
        shader.use();

        // --- Draw filled cube ---
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position) *
            glm::mat4_cast(orientation) * // NEW
            glm::scale(model, size);
        shader.setMat4("model", model);
        shader.setVec3("pickingColor", color);


        glBindVertexArray(sharedVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (selected) {
            glLineWidth(4.0f);

            if (state.getActiveTool() == GizmoTool::move) {
                // draw transform lines
                shader.setMat4("model", model); // or your transform
                glLineWidth(20.0f);
                glBindVertexArray(normalVAO);

                // Set color red for X axis lines
                shader.setVec3("pickingColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINES, 0, 4);

                // Set color green for Y axis lines
                shader.setVec3("pickingColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINES, 4, 4);

                // Set color blue for Z axis lines
                shader.setVec3("pickingColor", glm::vec3(0.0f, 0.0f, 1.0f));
                glDrawArrays(GL_LINES, 8, 4);

                glBindVertexArray(0);
            }

            if (state.getActiveTool() == GizmoTool::rotate) {
                glLineWidth(7.5f);

                glBindVertexArray(rotVAO);
                shader.setVec3("pickingColor", glm::vec3(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelXZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
                shader.setMat4("model", modelXZ);
                shader.setVec3("pickingColor", glm::vec3(0.0f, 0.0, 1.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);

                glm::mat4 modelYZ = model * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
                shader.setMat4("model", modelYZ);
                shader.setVec3("pickingColor", glm::vec3(0.0f, 1.0f, 0.0f));
                glDrawArrays(GL_LINE_LOOP, 0, segments);
            }
        }
    }

    static void cleanupSharedBuffers() {
        if (initialized) {
            glDeleteVertexArrays(1, &sharedVAO);
            glDeleteBuffers(1, &sharedVBO);
            initialized = false;
        }
    }

    bool intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& distance) const override {
        glm::vec3 boxMin = position - size * 0.5f;
        glm::vec3 boxMax = position + size * 0.5f;
        return rayIntersectsAABB(rayOrigin, rayDir, boxMin, boxMax, distance); 
    }

private:
    static GLuint edgeVAO;
    static GLuint edgeVBO;
    static GLuint normalVAO;
    static GLuint normalVBO;
    static GLuint rotVAO;
    static GLuint rotVBO;
    static unsigned int sharedVAO;
    static unsigned int sharedVBO;
    static unsigned int sharedEBO;
    static GLsizei indexCount; 
    static bool initialized;

    static void initSharedBuffers() {
        if (initialized) return;

        glGenVertexArrays(1, &sharedVAO);

        glGenBuffers(1, &sharedVBO);
        glGenBuffers(1, &sharedEBO);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        const float RADIUS = 1.0;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = RADIUS * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = RADIUS * std::cos(ySegment * PI);
                float zPos = RADIUS * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<GLsizei>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }

        glBindVertexArray(sharedVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        // initialize cube edge VAO and VBO TODO
        glGenVertexArrays(1, &edgeVAO);
        glGenBuffers(1, &edgeVBO);

        glBindVertexArray(edgeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeEdges), cubeEdges, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        // Normal VAO and VBO
        glGenVertexArrays(1, &normalVAO);
        glGenBuffers(1, &normalVBO);

        glBindVertexArray(normalVAO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        // Rotation gizmo VAO and VBO
        glGenVertexArrays(1, &rotVAO);
        glGenBuffers(1, &rotVBO);

        glBindVertexArray(rotVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rotVBO);
        glBufferData(GL_ARRAY_BUFFER, rotationVertices.size() * sizeof(float), rotationVertices.data(), GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        initialized = true;
    }
};

// Static member definitions
GLuint Cube::edgeVAO = 0;
GLuint Cube::edgeVBO = 0;
GLuint Cube::normalVAO = 0;
GLuint Cube::normalVBO = 0;
GLuint Cube::rotVAO = 0;
GLuint Cube::rotVBO = 0;
unsigned int Cube::sharedVAO = 0;
unsigned int Cube::sharedVBO = 0;
bool Cube::initialized = false;

GLuint Sphere::edgeVAO = 0;
GLuint Sphere::edgeVBO = 0;
GLuint Sphere::normalVAO = 0;
GLuint Sphere::normalVBO = 0;
GLuint Sphere::rotVAO = 0;
GLuint Sphere::rotVBO = 0;
unsigned int Sphere::sharedVAO = 0;
unsigned int Sphere::sharedVBO = 0;
unsigned int Sphere::sharedEBO = 0;
GLsizei Sphere::indexCount = 0;
bool Sphere::initialized = false;




