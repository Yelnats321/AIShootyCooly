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

struct sampleTag1 {};
struct sampleTag2 {};
struct sampleTag3 {};

using Components = ComponentList<sampleComponent1, sampleComponent2>;
using Tags = TagList<sampleTag1, sampleTag2, sampleTag3>;
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

	auto ent = em.createEntity();
	ent.addComponent<sampleComponent1>(3);
	ent.addComponent<sampleComponent2>("3");
	ent.getComponent<sampleComponent1>();
	ent.hasComponent<sampleComponent1>();
	ent.removeComponent<sampleComponent1>();

	std::cout<< ent.hasTag < sampleTag1>();
	ent.setTag<sampleTag1>(false);
	std::cout << ent.hasTag < sampleTag1>();
	ent.setTag<sampleTag1>(true);
	ent.setTag<sampleTag1>(true);
	std::cout << ent.hasTag < sampleTag1>();
	ent.setTag<sampleTag1>(false);
	std::cout << ent.hasTag < sampleTag1>();
	ent.setTag<sampleTag1>(true);
	ent.setTag<sampleTag2>(true);

	std::cout << em.getEntities<sampleComponent2, sampleTag1>().size();
	std::cout << em.getEntities<sampleComponent2, sampleTag2>().size();
	std::cout << em.getEntities<sampleTag2, sampleComponent2, sampleTag1>().size();
	std::cout << em.getEntities<sampleComponent1, sampleComponent2>().size();
	std::cout << em.getEntities<sampleComponent1, sampleComponent1>().size();
	std::cout << em.getEntities<sampleTag3, sampleComponent2, sampleTag1>().size();


	Model model("models/sponza-x/sponza.obj");
	model.setScale(0.01f, 0.01f, 0.01f);
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
