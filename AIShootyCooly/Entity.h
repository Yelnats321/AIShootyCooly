#pragma once

#include <type_traits>
#include <bitset>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/arithmetic.hpp>

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
namespace mpl = boost::mpl;

template <template <typename> class Container>
struct stdTupleCreator {
	template <class T, class R>
	struct toStdTuple;

	template <class... TTypes, class X>
	struct toStdTuple<std::tuple<TTypes...>, X> {
		using type = std::tuple<TTypes..., Container<X>>;
	};
};

template <class... Ts>
struct setCreator {
	using type =
		typename mpl::fold<
			mpl::vector<Ts...>,
			mpl::set0<>,
			mpl::insert<mpl::_1, mpl::_2>
		>::type;
};

template <class Set1, class Set2>
struct setIntersection {
	using type =
		typename mpl::fold<
		Set1,
		mpl::set0<>,
		mpl::if_<
			mpl::has_key<Set2, mpl::_2>,
			mpl::insert<mpl::_1, mpl::_2>,
			mpl::_1
		>
		>::type;
};

template <class Set, class Index>
constexpr int getIndex() {
	return mpl::distance<
			typename mpl::begin<Set>::type,
			typename mpl::find<Set, Index>::type
		>::type::value;
}

template <class Set, class Index>
struct indexOffset : mpl::integral_c<unsigned long long, 1ull << getIndex<Set, Index>()> {};

template <class Set, class PartialSet>
constexpr auto tupleBitset() {
	using type = 
		typename mpl::fold<
			PartialSet,
			mpl::integral_c<unsigned long long, 0>,
			//mpl::eval_if<
				//mpl::has_key<Set, mpl::_2>,
				mpl::plus<
					mpl::_1, 
					indexOffset<Set, mpl::_2>
				>//,
				//mpl::_1
			//>
		>::type;
	return std::bitset<mpl::size<Set>::value>(type::value);
}
}

template <class, class>
class EntityManager;

template<class Components, class Tags>
class Entity {
	using Manager = EntityManager<Components, Tags>;

	friend class Manager;

	Manager * manager = nullptr;
	EntityID id = EntityID(0);
	EntityVersion version = EntityVersion(0);
	std::bitset<Components::size> components;
	std::bitset<Tags::size> tags;

public:
	template <class Component, typename ... T>
	void addComponent(T&& ... t) {
		manager->addComponent<Component, T...>(*this, std::forward<T>(t)...);
	}

	template <class Component>
	Component & getComponent() {
		return manager->getComponent<Component>(*this);
	}

	template <class Component>
	const Component & getComponent() const {
		return manager->getComponent<Component>(*this);
	}

	template <class Component>
	bool hasComponent() const {
		return manager->hasComponent<Component>(*this);
	}

	template <class Component>
	void removeComponent() {
		manager->removeComponent<Component>(*this);
	}

	template <class Tag>
	bool hasTag() const {
		return manager->hasTag<Tag>(*this);
	}

	template <class Tag>
	void setTag(bool add) {
		manager->setTag<Tag>(*this, add);
	}
};

template<class... Ts>
struct ComponentList {
private:
	template<typename T>
	using container = std::unordered_map<EntityID, T>;
	using myStdTuple = stdTupleCreator<container>;
public:
	using setType = typename setCreator<Ts...>::type;
	using tupleType = typename mpl::fold<
		setType,
		std::tuple<>,
		myStdTuple::toStdTuple<mpl::_1, mpl::_2>
	>::type;
	static constexpr auto size = mpl::size<setType>::value;
};

template<class... Ts>
struct TagList {
	using container = std::unordered_set<EntityID>;

	using setType = typename setCreator<Ts...>::type;

	static constexpr auto size = mpl::size<setType>::value;
};

template<class ComponentList, class TagList>
class EntityManager {
	using MyEntity = Entity<ComponentList, TagList>;
	using EntityContainer = std::vector<MyEntity>;

	using ComponentSet = typename ComponentList::setType;
	using TagSet = typename TagList::setType;

	typename ComponentList::tupleType componentTuple;
	std::array<typename TagList::container, TagList::size> tagArray;

	struct EntityData { 
		bool inUse = false;
		MyEntity entity;
	};
	std::unordered_map<EntityID, EntityData> entityMap;

	void confirmEntity(const MyEntity & entity) const {
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
	EntityManager() {
		static_assert(mpl::size<setIntersection<ComponentSet, TagSet>::type>::value == 0,
					  "Tags and Components intersect");
	}

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
		static_assert(mpl::has_key<ComponentSet, Component>::value,
					  "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		std::get<iterPos>(componentTuple).insert_or_assign(entity.id, Component(t ...));
		entityMap[entity.id].entity.components[iterPos] = 1;
		entity.components[iterPos] = 1;
	}

	template <class Component>
	Component & getComponent(const MyEntity & entity) {
		return const_cast<Component &>
			(static_cast<const EntityManager<ComponentList, TagList> *>(this)
			 ->getComponent<Component>(entity));
	}

	template <class Component>
	const Component & getComponent(const MyEntity & entity) const {
		static_assert(mpl::has_key<ComponentSet, Component>::value,
					  "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		return std::get<iterPos>(componentTuple).at(entity.id);
	}

	template <class Component>
	bool hasComponent(const MyEntity & entity) const {
		static_assert(mpl::has_key<ComponentSet, Component>::value, "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		return entity.components[iterPos];
	}

	template <class Component>
	void removeComponent(MyEntity & entity) {
		static_assert(mpl::has_key<ComponentSet, Component>::value,
					  "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		std::get<iterPos>(componentTuple).erase(entity.id);
		entityMap[entity.id].entity.components[iterPos] = 0;
		entity.components[iterPos] = 0;
	}

	template <class Tag>
	bool hasTag(const MyEntity & entity) const {
		static_assert(mpl::has_key<TagSet, Tag>::value,
					  "Tag not part of TagSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<TagSet, Tag>();
		return tagArray[iterPos].count(entity.id) == 1;
	}

	template <class Tag>
	void setTag(MyEntity & entity, bool add) {
		static_assert(mpl::has_key<TagSet, Tag>::value,
					  "Tag not part of TagSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<TagSet, Tag>();
		if (add) {
			tagArray[iterPos].insert(entity.id);
		}
		else {
			tagArray[iterPos].erase(entity.id);
		}
		entityMap[entity.id].entity.tags[iterPos] = add;
		entity.tags[iterPos] = add;
	}

	template <class ... ThingsWanted>
	EntityContainer getEntities() {
		using ThingsWantedSet = typename setCreator<ThingsWanted...>::type;
		using ComponentsWantedSet = setIntersection<ThingsWantedSet, ComponentSet>::type;
		using TagsWantedSet = setIntersection<ThingsWantedSet, TagSet>::type;
		static_assert(mpl::size<ComponentsWantedSet>::value + mpl::size<TagsWantedSet>::value
					  == mpl::size<ThingsWantedSet>::value,
					  "Trying to get invalid things");
		constexpr auto componentBitset = tupleBitset<ComponentSet, ComponentsWantedSet>();
		constexpr auto tagBitset = tupleBitset<TagSet, TagsWantedSet>();
		EntityContainer entities;
		for (const auto & entity : entityMap) {
			if ((entity.second.entity.components & componentBitset) == componentBitset &&
				(entity.second.entity.tags & tagBitset) == tagBitset) {
				entities.emplace_back(entity.second.entity);
			}
		}
		return entities;
	}
};