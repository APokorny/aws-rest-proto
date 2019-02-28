#pragma once
#include <type_traits>
#include "kvasir/mpl/types/bool.hpp"

namespace arp {
namespace detail {
template <typename K, typename V, bool = std::is_empty<V>::value && !std::is_final<V>::value>
struct item;
template <typename K, typename V>
struct item<K, V, true> : V {
    constexpr item() {}
    template <typename T>
    explicit constexpr item(T&& t) : V(static_cast<T&&>(t)) {}
};
template <typename K, typename V>
struct item<K, V, false> {
    constexpr item() : data_() {}

    template <typename T>
    explicit constexpr item(T&& t) : data_(static_cast<T&&>(t)) {}
    V data_;
};

template <typename K>
struct is_key {
    template <typename T>
    struct f_impl;
    template <typename V>
    struct f_impl<item<K, V>> : kvasir::mpl::bool_<true> {};
    template <typename OtherK, typename V>
    struct f_impl<item<OtherK, V>> : kvasir::mpl::bool_<false> {};
    template <typename T>
    using f = f_impl<T>;
};

template <typename K, typename V>
constexpr V const& item_get(item<K, V, true> const& x) {
    return x;
}

template <typename K, typename V>
constexpr V& item_get(item<K, V, true>& x) {
    return x;
}

template <typename K, typename V>
constexpr V&& item_get(item<K, V, true>&& x) {
    return static_cast<V&&>(x);
}

template <typename K, typename V>
constexpr V const& item_get(item<K, V, false> const& x) {
    return x.data_;
}

template <typename K, typename V>
constexpr V& item_get(item<K, V, false>& x) {
    return x.data_;
}

template <typename K, typename V>
constexpr V&& item_get(item<K, V, false>&& x) {
    return static_cast<V&&>(x.data_);
}
template <typename T>
struct value_type_impl;
template <typename K, typename V>
struct value_type_impl<item<K, V>> {
    using type = V;
};

template <typename T>
using value_type = typename value_type_impl<T>::type;

}  // namespace detail
}  // namespace arp
