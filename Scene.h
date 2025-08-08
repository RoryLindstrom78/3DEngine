#include <vector>
#include "shader.h"
#include "Objects.h"
#include <iostream>

class Scene {
private:
	std::vector<Object*> objs;
	Object* selectedObject = nullptr;
public:
	void addObj(Object *obj) { objs.push_back(obj); }

	void draw() {
		for (const Object* obj : objs) {
			obj->draw();
		}
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