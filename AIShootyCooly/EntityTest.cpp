#include "stdafx.h"
#include "Entity.h"
#include <string>

namespace {
struct ComponentTest1 {
	int x_;
};

struct ComponentTest2 {
	std::string x_;
};

class ComponentTest3 {
	int min_, max_, curr_;

	void adjust() {
		if (curr_ < min_) {
			curr_ = min_;
		}
		if (curr_ > max_) {
			curr_ = max_;
		}
	}
public:
	ComponentTest3(int min, int max, int curr): min_(min), max_(max), curr_(curr) {}
	int add(int x) {
		curr_ += x;
		adjust();
		return curr_;
	}
	int sub(int x) {
		curr_ -= x;
		adjust();
		return curr_;
	}
	int get() const {
		return curr_;
	}
};

struct TagTest1 {};
struct TagTest2 {};
struct TagTest3 {};
}

TEST(Entity, Components) {
	EntityManager<ComponentList<ComponentTest1>, TagList<>> entManager;
	auto ent = entManager.createEntity();
	EXPECT_EQ(false, ent.hasComponent<ComponentTest1>());
	ent.addComponent<ComponentTest1>(ComponentTest1{2});
	EXPECT_EQ(true, ent.hasComponent<ComponentTest1>());
	EXPECT_EQ(2, ent.getComponent<ComponentTest1>().x_);
	ent.addComponent<ComponentTest1>(ComponentTest1{4});
	EXPECT_EQ(true, ent.hasComponent<ComponentTest1>());
	EXPECT_EQ(4, ent.getComponent<ComponentTest1>().x_);
	ent.removeComponent<ComponentTest1>();
	EXPECT_EQ(false, ent.hasComponent<ComponentTest1>());
}

TEST(Entity, Tags) {
	EntityManager<ComponentList<>, TagList<TagTest1, TagTest2>> entManager;
	auto ent = entManager.createEntity();
	EXPECT_EQ(false, ent.hasTag<TagTest1>());
	EXPECT_EQ(false, ent.hasTag<TagTest2>());
	ent.setTag<TagTest1>(false);
	ent.setTag<TagTest2>(false);
	EXPECT_EQ(false, ent.hasTag<TagTest1>());
	EXPECT_EQ(false, ent.hasTag<TagTest2>());
	ent.setTag<TagTest2>(true);
	EXPECT_EQ(false, ent.hasTag<TagTest1>());
	EXPECT_EQ(true, ent.hasTag<TagTest2>());
	ent.setTag<TagTest1>(true);
	EXPECT_EQ(true, ent.hasTag<TagTest1>());
	EXPECT_EQ(true, ent.hasTag<TagTest2>());
	ent.setTag<TagTest1>(false);
	ent.setTag<TagTest2>(false);
	EXPECT_EQ(false, ent.hasTag<TagTest1>());
	EXPECT_EQ(false, ent.hasTag<TagTest2>());
}

TEST(Entity, GetEntities) {
	EntityManager<ComponentList<ComponentTest1, ComponentTest2>, TagList<TagTest1, TagTest2>> entManager;
	auto ent1 = entManager.createEntity(), ent2 = entManager.createEntity();
	EXPECT_EQ(2, entManager.getEntities<>().size());
	EXPECT_EQ(0, entManager.getEntities<ComponentTest1>().size());
	EXPECT_EQ(0, entManager.getEntities<ComponentTest2>().size());
	EXPECT_EQ(0, entManager.getEntities<TagTest1>().size());
	EXPECT_EQ(0, entManager.getEntities<TagTest2>().size());
	ent1.addComponent<ComponentTest2>();
	EXPECT_EQ(0, entManager.getEntities<ComponentTest1>().size());
	EXPECT_EQ(1, entManager.getEntities<ComponentTest2>().size()); 
	EXPECT_EQ(0, (entManager.getEntities<ComponentTest2, ComponentTest1>().size()));
	EXPECT_EQ(0, (entManager.getEntities<ComponentTest1, ComponentTest2>().size()));
	EXPECT_EQ(0, entManager.getEntities<TagTest1>().size());
	EXPECT_EQ(0, entManager.getEntities<TagTest2>().size());
	ent2.setTag<TagTest1>(true);
	ent2.setTag<TagTest2>(true);
	EXPECT_EQ(1, entManager.getEntities<TagTest1>().size());
	EXPECT_EQ(1, entManager.getEntities<TagTest2>().size());
	EXPECT_EQ(1, (entManager.getEntities<TagTest1, TagTest2>().size()));
	EXPECT_EQ(ent2, (entManager.getEntities<TagTest1, TagTest2>().front()));
	ent1.setTag<TagTest1>(true);
	EXPECT_EQ(2, entManager.getEntities<TagTest1>().size());
	EXPECT_EQ(1, entManager.getEntities<TagTest2>().size());
	EXPECT_EQ(ent2, (entManager.getEntities<TagTest1, TagTest2>().front()));
}