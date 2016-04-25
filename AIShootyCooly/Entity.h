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
}

template <class, class>
class EntityManager;

template<class Components, class Tags>
class Entity {
	using Manager = EntityManager<Components, Tags>;

	friend class Manager;

	Manager * manager_ = nullptr;
	EntityID id_ = EntityID(0);
	EntityVersion version_ = EntityVersion(0);
	std::bitset<Components::size> components_;
	std::bitset<Tags::size> tags_;
public:
	template <class Component, typename ... T>
	void addComponent(T&& ... t);
	template <class Component>
	Component & getComponent();
	template <class Component>
	const Component & getComponent() const;
	template <class Component>
	bool hasComponent() const;
	template <class Component>
	void removeComponent();
	template <class Tag>
	bool hasTag() const;
	template <class Tag>
	void setTag(bool add);
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

	typename ComponentList::tupleType componentTuple_;
	std::array<typename TagList::container, TagList::size> tagArray_;

	struct EntityData { 
		bool inUse_ = false;
		MyEntity entity_;
	};
	std::unordered_map<EntityID, EntityData> entityMap_;

	void confirmEntity(const MyEntity & entity) const;
public:
	EntityManager() {
		static_assert(mpl::size<setIntersection<ComponentSet, TagSet>::type>::value == 0,
					  "Tags and Components intersect");
	}

	MyEntity createEntity();
	void removeEntity(MyEntity && entity);
	template <class Component, typename ... T>
	void addComponent(MyEntity & entity, T&& ...t);
	template <class Component>
	Component & getComponent(const MyEntity & entity);
	template <class Component>
	const Component & getComponent(const MyEntity & entity) const;
	template <class Component>
	bool hasComponent(const MyEntity & entity) const;
	template <class Component>
	void removeComponent(MyEntity & entity);
	template <class Tag>
	bool hasTag(const MyEntity & entity) const;
	template <class Tag>
	void setTag(MyEntity & entity, bool add);
	template <class ... ThingsWanted>
	EntityContainer getEntities();
};

#include "Entity.impl"