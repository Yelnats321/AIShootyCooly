#include "stdafx.h"
#include "System.h"
#include "Event.h"

namespace {
template <typename... Ts>
struct DummyList;

using Sys = SystemManager<DummyList<>, DummyList<>>;

void testSysFunc(double, Sys::EntityManager &) {}
}

TEST(System, AddSystem) {
	Sys sys;
	std::function<Sys::SystemSignature> func;
	sys.registerSystem(nullptr); // empty
	sys.registerSystem(func); // copy
	sys.registerSystem(std::move(func)); // move
	sys.registerSystem([](double, Sys::EntityManager &) {}); // lambda
	sys.registerSystem(testSysFunc);
}