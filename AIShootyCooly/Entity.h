#pragma once

#include <type_traits>
#include <bitset>
#include <boost/serialization/strong_typedef.hpp>

BOOST_STRONG_TYPEDEF(std::size_t, EntityID)
BOOST_STRONG_TYPEDEF(std::size_t, EntityVersion)

namespace std {
template <> struct hash<EntityID> {
	size_t operator()(const EntityID & id) const {
		return hash<size_t>()(id);
	}
};
}

namespace {
template<typename T>
using ComponentContainer = std::unordered_map<EntityID, T>;

template <class Component, class Rest>
struct TupleIndex;

template <class Component, class ... Rest>
struct TupleIndex<Component, std::tuple<ComponentContainer<Component>, Rest...>> {
	static constexpr std::size_t value = 0;
};

template <class Component, class First, class ... Rest>
struct TupleIndex<Component, std::tuple<First, Rest...>> {
	static constexpr std::size_t value = 1 + TupleIndex<Component, std::tuple<Rest...>>::value;
};

template <class ComponentList, class ... Rest>
struct TupleIndexForEach{
	static constexpr std::size_t value = 0;
};

template <class ComponentList, class First, class ... Rest>
struct TupleIndexForEach<ComponentList, First, Rest...> {
	static constexpr std::size_t value = (1 << TupleIndex<First, ComponentList::type>::value)
		+ TupleIndexForEach<ComponentList, Rest...>::value;
};

template <class ComponentList, class ... Components>
auto tupleBitset() {
	return std::bitset<ComponentList::size>(TupleIndexForEach<ComponentList, Components...>::value);
}
}

template <class Components>
class EntityManager;

template<class Components>
class Entity {
	using Manager = EntityManager<Components>;

	friend class Manager;

	Manager * manager = nullptr;
	EntityID id = EntityID(0);
	EntityVersion version = EntityVersion(0);
	std::bitset<Components::size> components;
};

template<class ... T>
struct ComponentList {
	using type = std::tuple<ComponentContainer<T>...>;

	static constexpr auto size = std::tuple_size<type>::value;
};

template<class ComponentList>
class EntityManager {
	using MyEntity = Entity<ComponentList>;
	using EntityContainer = std::vector<MyEntity>;

	typename ComponentList::type components;
	struct EntityData { 
		bool inUse = false;
		MyEntity entity;
	};
	std::unordered_map<EntityID, EntityData> entityMap;


	void confirmEntity(const MyEntity & entity) {
		if (entity.manager != this) {
			throw std::invalid_argument("Entity does not belong to this EntityManager");
		}
		auto it = entityMap.find(entity.id);
		if (it == entityMap.end()) {
			throw std::invalid_argument("Entity does not exist");
		}
		if (!it->second.inUse) {
			throw std::invalid_argument("Entity is already dead");
		}
		if (it->second.entity.version != entity.version) {
			throw std::invalid_argument("Entity has invalid version");
		}
		if (it->second.entity.components != entity.components) {
			throw std::invalid_argument("Entity has incorrect component bitset");
		}
	}
public:
	EntityManager() {};

	MyEntity createEntity() {
		EntityID id(0);
		while (true) {
			if (!entityMap[id].inUse) {
				entityMap[id].inUse = true;
				entityMap[id].entity.id = id;
				entityMap[id].entity.manager = this;
				entityMap[id].entity.components.reset();
				return entityMap[id].entity;
			}
			else {
				id++;
			}
		}
	}
	void removeEntity(MyEntity && entity) {
		confirmEntity(entity);
		entityMap[entity.id].inUse = false;
		++entityMap[entity.id].entity.version;
	}

	template <class Component, typename ... T>
	void addComponent(MyEntity & entity, T&& ... t) {
		confirmEntity(entity);
		std::get<ComponentContainer<Component>>(components).insert_or_assign(entity.id, Component(t ...));
		entityMap[entity.id].entity.components[TupleIndex<Component, ComponentList::type>::value] = 1;
		entity.components[TupleIndex<Component, ComponentList::type>::value] = 1;
	}

	template <class Component>
	Component & getComponent(MyEntity entity) {
		confirmEntity(entity);
		return std::get<ComponentContainer<Component>>(components).at(entity.id);
	}

	template <class Component>
	void removeComponent(MyEntity & entity) {
		confirmEntity(entity);
		std::get<ComponentContainer<Component>>(components).erase(entity.id);
		entityMap[entity.id].entity.components[TupleIndex<Component, ComponentList::type>::value] = 0;
		entity.components[TupleIndex<Component, ComponentList::type>::value] = 0;
	}

	template <class ... ComponentsWanted>
	EntityContainer getEntities() {
		// temporary generated here, yuck!
		auto bitset = tupleBitset<ComponentList, ComponentsWanted...>();
		EntityContainer entities;
		for (const auto & entity : entityMap) {
			if ((entity.second.entity.components & bitset) == bitset) {
				entities.emplace_back(entity.second.entity);
			}
		}
		return entities;
	}
};

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
using MyEntityManager = EntityManager<Components>;