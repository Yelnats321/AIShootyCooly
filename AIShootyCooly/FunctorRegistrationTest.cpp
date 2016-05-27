#include "stdafx.h"
#include "System.h"
#include "Event.h"

namespace {
template <typename... Ts>
struct DummyList;

using Sys = SystemManager<DummyList<>, DummyList<>>;

void testSysFunc(double, Sys::EntityManager &) {}

struct DummyEvent {};
using Eve = EventManager<EventList<DummyEvent>>;

void testEveFunc(DummyEvent) {};
}

TEST(System, AddSystem) {
	Sys sys;
	std::function<Sys::SystemSignature> func;
	sys.registerSystem(nullptr); // empty Maybe shouldn't be allowed to?
	sys.registerSystem(func); // copy
	sys.registerSystem(std::move(func)); // move
	sys.registerSystem([](double, Sys::EntityManager &) {}); // lambda
	sys.registerSystem(testSysFunc);
}

TEST(Event, AddEventListener) {
	Eve eve;
	std::function<void(DummyEvent)> func;
	eve.addListener(func); // copy
	eve.addListener(std::move(func)); // move
	eve.addListener([](DummyEvent) {}); // lambda
	eve.addListener(testEveFunc);
}
