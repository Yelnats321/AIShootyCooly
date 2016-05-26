#pragma once

#include "MplUtil.h"

#include <callable_traits/args.hpp>
#include <callable_traits/arg_at.hpp>
#include <boost/mpl/size.hpp>
#include <functional>

template<class... Ts>
struct EventList {
private:
	template<typename T>
	using container = std::vector<std::function<void(T)>>;
	using myStdTuple = stdTupleCreator<container>;
public:
	using setType = typename setCreator<Ts...>::type;
	using tupleType = typename mpl::fold<
		setType,
		std::tuple<>,
		typename myStdTuple::template toStdTuple<mpl::_1, mpl::_2>
	>::type;
	static constexpr auto size = mpl::size<setType>::value;
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