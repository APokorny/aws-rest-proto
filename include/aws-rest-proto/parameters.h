#pragma once
#include "kvasir/mpl/algorithm/find_if.hpp"
#include "kvasir/mpl/sequence/front.hpp"
#include "kvasir/mpl/sequence/size.hpp"
#include "aws-rest-proto/detail/item.h"

namespace arp {
template <typename... Params>
struct params : Params... {
    constexpr params() = default;

    static_assert(kvasir::mpl::call<kvasir::mpl::all<detail::is_item>,Params...>::value, "All Params must be items");
    using this_t = params<Params...>;
    template <typename ...Others,
              typename = std::enable_if_t<
                  std::is_same<this_t, params<std::decay_t<Others>...> >::value>>
    constexpr params(Others&&... other) : Params(static_cast<Others&&>(other))... {}
    template <typename ...Others,
              typename = std::enable_if_t<
                  std::is_same<this_t, params<std::decay_t<Others>...> >::value>>
    constexpr params(params<Others...> const& other) : Params(static_cast<Others&&>(other))... {}
};


template<typename Name, typename Parameters>
struct value_type;

template<typename Name, typename... Params>
struct value_type<Name, params<Params...>> {
    using type = kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<Name>, kvasir::mpl::front<>>, Params...>;
};

template <typename Key, typename... Params,
          typename ActualType = kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<Key>, kvasir::mpl::front<>>, Params...>>
constexpr auto const& get(params<Params...> const& parameter_map) {
    return item_get(static_cast<ActualType const&>(parameter_map));
}

template <typename Key, typename... Params,
          typename ActualType = kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<Key>, kvasir::mpl::front<>>, Params...>>
constexpr auto& get(params<Params...>& parameter_map) {
    return item_get(static_cast<ActualType&>(parameter_map));
}

template <typename Name, typename Parameters>
struct has_key;

template <typename Name, typename... Params>
struct has_key<Name, params<Params...>>
    : kvasir::mpl::bool_<kvasir::mpl::call<kvasir::mpl::find_if<detail::is_key<Name>, kvasir::mpl::size<>>, Params...>::value != 0> {};
}  // namespace arp
