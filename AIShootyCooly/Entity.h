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

	Manager * manager_ = nullptr;
	EntityID id_ = EntityID(0);
	EntityVersion version_ = EntityVersion(0);
	std::bitset<Components::size> components_;
	std::bitset<Tags::size> tags_;

public:
	template <class Component, typename ... T>
	void addComponent(T&& ... t) {
		manager_->addComponent<Component, T...>(*this, std::forward<T>(t)...);
	}

	template <class Component>
	Component & getComponent() {
		return manager_->getComponent<Component>(*this);
	}

	template <class Component>
	const Component & getComponent() const {
		return manager_->getComponent<Component>(*this);
	}

	template <class Component>
	bool hasComponent() const {
		return manager_->hasComponent<Component>(*this);
	}

	template <class Component>
	void removeComponent() {
		manager_->removeComponent<Component>(*this);
	}

	template <class Tag>
	bool hasTag() const {
		return manager_->hasTag<Tag>(*this);
	}

	template <class Tag>
	void setTag(bool add) {
		manager_->setTag<Tag>(*this, add);
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

	typename ComponentList::tupleType componentTuple_;
	std::array<typename TagList::container, TagList::size> tagArray_;

	struct EntityData { 
		bool inUse_ = false;
		MyEntity entity_;
	};
	std::unordered_map<EntityID, EntityData> entityMap_;

	void confirmEntity(const MyEntity & entity) const {
		if (entity.manager_ != this) {
			throw std::invalid_argument("Entity does not belong to this EntityManager");
		}
		auto it = entityMap_.find(entity.id_);
		if (it == entityMap_.end()) {
			throw std::invalid_argument("Entity does not exist");
		}
		if (!it->second.inUse_) {
			throw std::invalid_argument("Entity is already dead");
		}
		if (it->second.entity_.version_ != entity.version_) {
			throw std::invalid_argument("Entity has invalid version");
		}
		if (it->second.entity_.components_ != entity.components_) {
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
			if (!entityMap_[id].inUse_) {
				entityMap_[id].inUse_ = true;
				entityMap_[id].entity_.id_ = id;
				entityMap_[id].entity_.manager_ = this;
				entityMap_[id].entity_.components_.reset();
				return entityMap_[id].entity_;
			}
			else {
				id++;
			}
		}
	}

	void removeEntity(MyEntity && entity) {
		confirmEntity(entity);
		entityMap_[entity.id].inUse_ = false;
		++entityMap_[entity.id].entity_.version_;
	}

	template <class Component, typename ... T>
	void addComponent(MyEntity & entity, T&& ...t) {
		static_assert(mpl::has_key<ComponentSet, Component>::value,
					  "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		std::get<iterPos>(componentTuple_).insert_or_assign(entity.id_, Component(t...));
		entityMap_[entity.id_].entity_.components_[iterPos] = 1;
		entity.components_[iterPos] = 1;
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
		return std::get<iterPos>(componentTuple_).at(entity.id_);
	}

	template <class Component>
	bool hasComponent(const MyEntity & entity) const {
		static_assert(mpl::has_key<ComponentSet, Component>::value, "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		return entity.components_[iterPos];
	}

	template <class Component>
	void removeComponent(MyEntity & entity) {
		static_assert(mpl::has_key<ComponentSet, Component>::value,
					  "Component not part of ComponentSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<ComponentSet, Component>();
		std::get<iterPos>(componentTuple_).erase(entity.id_);
		entityMap_[entity.id_].entity_.components_[iterPos] = 0;
		entity.components_[iterPos] = 0;
	}

	template <class Tag>
	bool hasTag(const MyEntity & entity) const {
		static_assert(mpl::has_key<TagSet, Tag>::value,
					  "Tag not part of TagSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<TagSet, Tag>();
		return tagArray_[iterPos].count(entity.id_) == 1;
	}

	template <class Tag>
	void setTag(MyEntity & entity, bool add) {
		static_assert(mpl::has_key<TagSet, Tag>::value,
					  "Tag not part of TagSet");
		confirmEntity(entity);
		constexpr auto iterPos = getIndex<TagSet, Tag>();
		if (add) {
			tagArray_[iterPos].insert(entity.id_);
		}
		else {
			tagArray_[iterPos].erase(entity.id_);
		}
		entityMap_[entity.id_].entity_.tags_[iterPos] = add;
		entity.tags_[iterPos] = add;
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
		for (const auto & entity : entityMap_) {
			if ((entity.second.entity_.components_ & componentBitset) == componentBitset &&
				(entity.second.entity_.tags_ & tagBitset) == tagBitset) {
				entities.emplace_back(entity.second.entity_);
			}
		}
		return entities;
	}
};