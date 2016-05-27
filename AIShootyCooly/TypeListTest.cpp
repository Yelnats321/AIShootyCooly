#include "stdafx.h"
#include "Entity.h"
#include "Event.h"

namespace {
struct type1 {};
struct type2 {};
struct type3 {};
};

#include <iostream>
TEST(TypeList, EventType) {
	{
		using et = EventList<>;
		static_assert(et::size == 0, "");
	}
	{
		using et = EventList<type1, type1>;
		static_assert(et::size == 1, "");
	}
	{
		using et = EventList<type1, type2, type1>;
		static_assert(et::size == 2, "");
	}
	{
		using et = EventList<type1, type2, type3, type1>;
		static_assert(et::size == 3, "");
	}
	{
		using et = EventList<type1, type1, type1, type1>;
		static_assert(et::size == 1, "");
	}
}

TEST(TypeList, ComponentType) {

}

TEST(TypeList, TagType) {

}