#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Scene.h"
#include "Objects.h"

// settings
const unsigned int width = 800; 
const unsigned int height = 600;

class ColorPicker {
public:
	ColorPicker(Scene& scne, Shader& shdr, Camera& cmra) : scene(scne), shader(shdr), camera(cmra) {
		initSharedBuffers();
	}

	void renderPickingPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);
		glViewport(0, 0, width, height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		for (auto* obj : scene.getObjs()) {
			int id = obj->ID;
			// We encode an RGB color based on the object's ID
			glm::vec3 color = glm::vec3(
				((id >> 16) & 0xFF) / 255.0f,
				((id >> 8) & 0xFF) / 255.0f,
				(id & 0xFF) / 255.0f
			);
			shader.setVec3("pickingColor", color);
			obj->draw(shader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int getObjectIDAtPixel(int mouseX, int mouseY, int windowHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

		unsigned char pixel[3];
		glReadPixels(mouseX, windowHeight - mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int pickedID = (pixel[0] << 16) | (pixel[1] << 8) | pixel[2];

		// 0 means background
		if (pickedID == 0)
			return -1;

		return pickedID;
	}

private:
	static GLuint pickingFBO, pickingTexture, pickingDepth;
	static bool initialized;
	Scene& scene;
	Shader& shader;
	Camera& camera;

	static void initSharedBuffers() {
		if (initialized) return;

		glGenFramebuffers(1, &pickingFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

		glGenTextures(1, &pickingTexture);
		glBindTexture(GL_TEXTURE_2D, pickingTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTexture, 0);

		glGenRenderbuffers(1, &pickingDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, pickingDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingDepth);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Picking FBO not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		initialized = true;
	}
};

GLuint ColorPicker::pickingFBO = 0;
GLuint ColorPicker::pickingTexture = 0;
GLuint ColorPicker::pickingDepth = 0;
bool ColorPicker::initialized = false;
