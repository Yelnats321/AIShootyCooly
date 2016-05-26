#include "stdafx.h"
#include "Renderer.h"
#include "Entity.h"
#include "System.h"
#include "Physics.h"
#include "Model.h"
#include "Event.h"

#include <vld.h>

using Components = ComponentList<>;
using Tags = TagList<>;
using MyEntityManager = EntityManager<Components, Tags>;

using namespace physx;


int main(int argc, char ** argv) {
	::testing::InitGoogleTest(&argc, argv);
	if (RUN_ALL_TESTS() != 0) {
		std::cin.get();
	}
	EventManager<EventList<>> ev;
	Renderer renderer;
	Physics::getInstance();

	MyEntityManager em;
	SystemManager<Components, Tags> sys;

	sys.registerSystem([](double dt, MyEntityManager &) {
		Physics::getInstance().step(dt);
	});

	Model model("models/3rdparty/sponza-x/sponza.obj");
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
