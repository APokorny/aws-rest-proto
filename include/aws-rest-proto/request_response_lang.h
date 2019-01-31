#pragma once

namespace arp {
namespace t {
template <typename Service, typename... Ts>
struct response {};
template <typename Service, typename... Ts>
struct request {};
template <typename Value>
struct param {};
template <typename Value>
struct object {};
struct any_other {};
template <typename... Ts>
struct payload {};
}  // namespace t

template <typename... Ts>
t::payload<Ts...> payload(Ts &&...) {
    return {};
}

template <template Service, typename... Ts>
t::request<Service, Ts...> request(Ts &&...) {
    return {};
}

template <typename Service, typename... Ts>
t::response<Service, Ts...> response(Ts &&...) {
    return {};
}

template <typename NameT, typename... Ts>
t::object<Service, Ts...> object(NameT, Ts &&...) {
    return {};
}

template <typename Name>
t::service<Name> service(Name &&) {
    return {};
}

const t::any_other any_other;

}  // namespace arp
