#pragma once
#include <vector>
#include "shader.h"
#include "Objects.h"
#include <iostream>

enum class MoveAxis { None, X, Y, Z };

struct GizmoState {
	bool isMoving = false;
	MoveAxis ActiveAxis = MoveAxis::None;
	glm::vec3 initialClickPos;
};

class Scene {
private:
	Object* selectedObject = nullptr;
	int numObjects = 0;
	std::vector<Object*> objs;
public:
	void addObj(Object *obj) { 
		objs.push_back(obj); 
		obj->ID = ++numObjects;
	}

	std::vector<Object*> getObjs() { return objs; }

	Object* getSelectedObj() { return selectedObject; }

	void draw(Shader& shader) {
		for (const Object* obj : objs) {
			obj->draw(shader);
		}
	}

	void selectObject(Object* obj) {
		selectedObject = obj;
	}

	void selectObjectFromRay(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir) {
		
		// if an object is already selected we want to first check if we're clicking the object's move arrows
		if (selectedObject) {
			selectLineFromRay(rayOrigin, rayDir);
		}
		
		float closestDistance = std::numeric_limits<float>::max();
		Object* closestObject = nullptr;

		for (auto& obj : objs) {
			float distance = 0.0f;
			if (obj->intersectsRay(rayOrigin, rayDir, distance)) {
				if (distance < closestDistance) {
					closestDistance = distance;
					closestObject = obj;
				}
			}
		}

		selectedObject = closestObject;
		if (selectedObject) {
			if (!selectedObject->isSelected()) selectedObject->toggleSelected();
		}


		if (selectedObject != nullptr) {
			std::cout << "selected" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
		}
	}

	void selectLineFromRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
		std::cout << "code this here" << std::endl;
	}
};