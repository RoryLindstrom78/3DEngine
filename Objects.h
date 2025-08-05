#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

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


