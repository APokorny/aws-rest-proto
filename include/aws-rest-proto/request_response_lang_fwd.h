#pragma once
namespace arp {
namespace t {
template <typename Service, typename Name, typename... Ts>
struct response;
template <typename Ts>
struct response_ref;
template <typename Service, typename Name, typename... Ts>
struct request;
template <typename Name, typename Value>
struct param;
template <typename Name, typename... Ts>
struct object;
struct any_other;
template <typename... Ts>
struct payload;
template <typename... Ts>
struct error_response;
template <typename Name, typename HostPrefix>
struct service;
}  // namespace t
}  // namespace arp
