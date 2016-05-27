#pragma once

#include <callable_traits/args.hpp>
#include <callable_traits/arg_at.hpp>
#include <functional>
#include <boost/hana.hpp>

namespace hana = boost::hana;

template<class... Ts>
struct EventList {
private:
public:
	static constexpr auto set = hana::to_set(hana::tuple_t<Ts...>);
	static constexpr auto size = decltype(hana::size(set))::value;
};

//std::tuple<std::vector<std::function<void(EVENT)>>>
template <class EventList>
class EventManager {
public:
	template <
		typename Func
		, typename Event = callable_traits::arg_at<0, Func>
		, typename = std::enable_if_t<std::tuple_size<callable_traits::args<Func>>::value == 1>
	>
	std::shared_ptr<std::function<void(Event)>> addListener(Func&&);
	//void removeListener(? ? ? );
	//void notify();
	//Design decisions:
	// how to add and remove listener? cant equate std::function
	// should notifications be constants? in the common use case, no need to copy
	// 
};

#include "Event.impl"