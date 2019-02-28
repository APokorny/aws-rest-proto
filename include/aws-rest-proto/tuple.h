#pragma once
#include "kvasir/mpl/algorithm/find_if.hpp"
#include "kvasir/mpl/sequence/front.hpp"
#include "kvasir/mpl/sequence/size.hpp"
#include "aws-rest-proto/detail/item.h"

namespace arp {
namespace detail {
struct from_other {};
template <typename Index, typename... Params>
struct tuple_impl;
template <int... Is, typename... Params>
struct tuple_impl<std::integer_sequence<int, Is...>, Params...> : item<kvasir::mpl::int_<Is>, Params>... {
    constexpr tuple_impl() = default;
    template <typename Other>
    explicit constexpr tuple_impl(detail::from_other, Other&& other)
        : item<kvasir::mpl::int_<Is>, Params>(detail::item_get<kvasir::mpl::int_<Is>>(static_cast<Other&&>(other)))... {}

    template <typename... Yn>
    explicit constexpr tuple_impl(Yn&&... yn) : item<kvasir::mpl::int_<Is>, Yn>(static_cast<Yn&&>(yn))... {}
};
}  // namespace detail
template <typename... Params>
struct tuple : detail::tuple_impl<std::make_integer_sequence<int, sizeof...(Params)>, Params...> {
    constexpr tuple() = default;
    using Base = detail::tuple_impl<std::make_integer_sequence<int, sizeof...(Params)>, Params...>;
    template <typename Other,
              typename = typename std::enable_if<std::is_same<typename std::decay<Other>::type, tuple<Params...>>::value, void>::type>
    constexpr tuple(Other&& other) : Base(detail::from_other{}, static_cast<Other&&>(other)) {}

    template <typename... Yn>
    explicit constexpr tuple(Yn&&... yn) : Base(static_cast<Yn&&>(yn)...) {}
};

template <
    int Index, typename... Ts,
    typename ActualType = kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<kvasir::mpl::int_<Index>>, kvasir::mpl::front<>>, Ts...>>
constexpr auto const& get(tuple<Ts...> const& t) {
    return item_get(static_cast<ActualType const&>(t));
}

template <
    int Index, typename... Ts,
    typename ActualType = kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<kvasir::mpl::int_<Index>>, kvasir::mpl::front<>>, Ts...>>
constexpr auto& get(tuple<Ts...>& t) {
    return item_get(static_cast<ActualType&>(t));
}

}  // namespace arp
