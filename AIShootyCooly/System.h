#pragma once
#include <functional>
#include <vector>

template<class, class>
class EntityManager;

template<class Components, class Tags>
class SystemManager {
	using MyEntityManager = EntityManager<Components, Tags>;
	using SystemSignature = void(double, MyEntityManager);
	std::vector<std::function<SystemSignature>> systems;
public:
	SystemManager() {}
	void registerSystem(std::function<SystemSignature> system) {
		systems.emplace_back(std::move(system));
	}
	void step(double dt, MyEntityManager & manager) {
		for (auto && sys : systems) {
			sys(dt, manager);
		}
	}
};