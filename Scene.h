#include <vector>
#include "shader.h"
#include "Objects.h"

class Scene {
private:
	std::vector<Object*> objs;
public:
	void addObj(Object *obj) { objs.push_back(obj); }

	void draw() {
		for (const Object* obj : objs) {
			obj->draw();
		}
	}
};