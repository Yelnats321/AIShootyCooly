#pragma once
#include <boost/mpl/fold.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/vector.hpp>

namespace mpl = boost::mpl;

template <class... Ts>
struct setCreator {
	using type =
		typename mpl::fold<
			mpl::vector<Ts...>,
			mpl::set0<>,
			mpl::insert<mpl::_1, mpl::_2>
		>::type;
};

// Creates a struct that converts a variadic template
// to be a tuple of containers
// example: <int, float, double>, container = std::vector<T> ->
//		std::tuple<std::vector<int>, std::vector<float>, std::vector<double>>
template <template <typename> class Container>
struct stdTupleCreator {
	template <class T, class R>
	struct toStdTuple;

	template <class... TTypes, class X>
	struct toStdTuple<std::tuple<TTypes...>, X> {
		using type = std::tuple<TTypes..., Container<X>>;
	};
};