#pragma once
#include "aws-rest-proto/detail/item.h"
#include "kvasir/mpl/types/list.hpp"

namespace arp {
namespace detail {
template <typename Key, typename V>
using assignment = arp::detail::item<Key, V>;
}
namespace t {
template <char... Vs>
struct string {
    template <typename V>
    constexpr detail::assignment<string<Vs...>, std::decay_t<V>> operator=(V&& value) const {
        return detail::assignment<string<Vs...>, std::decay_t<V>>(static_cast<V&&>(value));
    }
};
}  // namespace t
namespace detail {
template <char... s>
constexpr char const string_storage[sizeof...(s) + 1] = {s..., '\0'};
template <char... s>
char const* c_str(t::string<s...>) {
    return string_storage<s...>;
}

static constexpr int select_concat_size(const int i) { return i <= 4 ? 0 : i <= 16 ? 1 : 2; }

template <int I>
struct concat_select;
template <>
struct concat_select<0> {  // 4
    template <typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>>
    struct f;

    template <char... V0s, char... V1s, char... V2s, char... V3s>
    struct f<t::string<V0s...>, t::string<V1s...>, t::string<V2s...>, t::string<V3s...>> {
        using type = t::string<V0s..., V1s..., V2s..., V3s...>;
    };
};

template <>
struct concat_select<1> {  // 16
    template <typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>,
              typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>,
              typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>, typename = t::string<>,
              typename = t::string<>>
    struct f;

    template <char... V0s, char... V1s, char... V2s, char... V3s, char... V4s, char... V5s, char... V6s, char... V7s, char... V8s,
              char... V9s, char... V10s, char... V11s, char... V12s, char... V13s, char... V14s, char... V15s>
    struct f<t::string<V0s...>, t::string<V1s...>, t::string<V2s...>, t::string<V3s...>, t::string<V4s...>, t::string<V5s...>,
             t::string<V6s...>, t::string<V7s...>, t::string<V8s...>, t::string<V9s...>, t::string<V10s...>, t::string<V11s...>,
             t::string<V12s...>, t::string<V13s...>, t::string<V14s...>, t::string<V15s...>> {
        using type = t::string<V0s..., V1s..., V2s..., V3s..., V4s..., V5s..., V6s..., V7s..., V8s..., V9s..., V10s..., V11s..., V12s...,
                               V13s..., V14s..., V15s...>;
    };
};

template <>
struct concat_select<2> {  // more than 16
    /*template <typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename,
              typename, typename, typename, typename, typename>
    struct f;*/
    template <typename... Ts>
    struct f;

    template <char... V0s, char... V1s, char... V2s, char... V3s, char... V4s, char... V5s, char... V6s, char... V7s, char... V8s,
              char... V9s, char... V10s, char... V11s, char... V12s, char... V13s, char... V14s, char... V15s, typename... Ls>
    struct f<t::string<V0s...>, t::string<V1s...>, t::string<V2s...>, t::string<V3s...>, t::string<V4s...>, t::string<V5s...>,
             t::string<V6s...>, t::string<V7s...>, t::string<V8s...>, t::string<V9s...>, t::string<V10s...>, t::string<V11s...>,
             t::string<V12s...>, t::string<V13s...>, t::string<V14s...>, t::string<V15s...>, Ls...> {
        using type = typename concat_select<select_concat_size(
            sizeof...(Ls) + 1)>::template f<t::string<V0s..., V1s..., V2s..., V3s..., V4s..., V5s..., V6s..., V7s..., V8s..., V9s...,
                                                      V10s..., V11s..., V12s..., V13s..., V14s..., V15s..., V15s...>,
                                            Ls...>::type;
    };
};
}  // namespace detail

template <typename... Ts>
using concat_t = typename detail::concat_select<detail::select_concat_size(sizeof...(Ts))>::template f<Ts...>::type;
template <typename... Ts>
auto concat(Ts...) -> typename detail::concat_select<detail::select_concat_size(sizeof...(Ts))>::template f<Ts...>::type {
    return {};
}
template <typename... Ts>
auto concat_list(kvasir::mpl::list<Ts...>) -> typename detail::concat_select<detail::select_concat_size(sizeof...(Ts))>::template f<Ts...>::type {
    return {};
}
}  // namespace arp
