#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

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

// Cube vertex data
static float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

// Base object class
class Object {
public:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;

    Object(glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 sze = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f))
        : position(pos), size(sze), rotation(rot) {
    }

    virtual ~Object() = default;
    virtual void draw() const = 0;
    virtual bool intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& distance) const = 0;
};

// Cube class with shared VAO/VBO
class Cube : public Object {
public:
    Shader& shader;

    Cube(Shader& shdr,
        glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 sze = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f))
        : Object(pos, sze, rot), shader(shdr) {
        initSharedBuffers();
    }

    void draw() const override {
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, size);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        shader.setMat4("model", model);

        glBindVertexArray(sharedVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
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
    static unsigned int sharedVAO;
    static unsigned int sharedVBO;
    static bool initialized;

    static void initSharedBuffers() {
        if (initialized) return;

        glGenVertexArrays(1, &sharedVAO);
        glGenBuffers(1, &sharedVBO);

        glBindVertexArray(sharedVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        initialized = true;
    }
};

// Static member definitions
unsigned int Cube::sharedVAO = 0;
unsigned int Cube::sharedVBO = 0;
bool Cube::initialized = false;


