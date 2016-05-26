#pragma once
#include <functional>
#include <vector>

template<class, class>
class EntityManager;

template<class Components, class Tags>
class SystemManager {
public:
	using EntityManager = EntityManager<Components, Tags>;
	using SystemSignature = void(double, EntityManager &);

private:
	std::vector<std::function<SystemSignature>> systems_;
public:	
	template <class Func>
	void registerSystem(Func && system) {
		systems_.emplace_back(std::move(system));
	}
	void step(double dt, EntityManager & manager) {
		for (auto && sys : systems_) {
			sys(dt, manager);
		}
	}
};
