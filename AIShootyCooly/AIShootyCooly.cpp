#include "stdafx.h"
#include "Renderer.h"
#include "Entity.h"
#include "System.h"
#include "Physics.h"
#include "Model.h"

#include <vld.h>

// Examples

struct sampleComponent1 {
	int x;
	sampleComponent1(int _x): x(_x) {};
};
struct sampleComponent2 {
	std::string text;
	sampleComponent2(std::string _text): text(_text) {};
};

using Components = ComponentList<sampleComponent1, sampleComponent2>;
using Tags = TagList<>;
using MyEntityManager = EntityManager<Components, Tags>;

using namespace physx;

int main() {
	Renderer renderer;
	Physics::getInstance();

	MyEntityManager em;
	SystemManager<Components, Tags> sys;

	sys.registerSystem([](double dt, MyEntityManager &) {
		Physics::getInstance().step(dt);
	});

	Model model("models/sponza-x/sponza.obj");
	int width, height;
	unsigned char* image =
		SOIL_load_image("models/sponza-x/textures/background.tga", &width, &height, 0, SOIL_LOAD_RGB);
	SOIL_free_image_data(image);

	auto oldTime = glfwGetTime();
	while (renderer.isOpen()) {
		auto newTime = glfwGetTime();
		auto dt = newTime - oldTime;
		oldTime = newTime;
		sys.step(dt, em);
		renderer.updateLocation(float(dt));
		renderer.draw(model);
	}

	return 0;
}
