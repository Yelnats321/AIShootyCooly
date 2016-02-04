#include "stdafx.h"
#include "Renderer.h"
#include "Entity.h"

#include <vld.h>

using namespace physx;

int main() {
	MyEntityManager em;
	Renderer renderer;
	auto ent = em.createEntity();
	em.addComponent<sampleComponent1>(ent, 5);
	auto component = em.getComponent<sampleComponent1>(ent);
	auto ents = em.getEntities<sampleComponent2>();
	while (renderer.isOpen()) {
		renderer.draw();
	}

	return 0;
}
