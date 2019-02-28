#pragma once
#include <aws-rest-proto/detail/string.h>
#include <aws-rest-proto/request_response_lang_fwd.h>
#include <aws-rest-proto/detail/item.h>
#include <aws-rest-proto/detail/request_response_lang.h>
#include <type_traits>

namespace arp {
namespace t {

template <typename T>
struct assignable {
    template <typename OtherT>
    constexpr auto operator=(OtherT &&other) const -> detail::item<T, std::decay_t<OtherT>> {
        return detail::item<T, std::decay_t<OtherT>>(static_cast<OtherT &&>(other));
    }
};
template <typename Service, typename Name, typename... Ts>
struct response : assignable<response<Service, Name, Ts...>> {
    using assignable<response<Service, Name, Ts...>>::operator=;
};
template <typename T>
struct response_ref : assignable<response_ref<T>> {
    using assignable<response_ref<T>>::operator=;
};
template <typename Service, typename Name, typename... Ts>
struct request {};
template <typename Name, typename Value>
struct param {};
template <typename Name, typename Expression>
struct ensure {};
template <typename Name, typename... Ts>
struct object {};
struct any_other : assignable<any_other> {
    using assignable<any_other>::operator=;
};
struct on_failure : assignable<on_failure> {
    using assignable<on_failure>::operator=;
};
template <typename... Ts>
struct payload {};
template <typename... Ts>
struct error_response {};
template <typename Name, typename HostPrefix>
struct service {};
}  // namespace t

template <typename... Ts>
constexpr auto payload(Ts &&...) -> t::payload<Ts...>;

template <typename Service, typename... Ts>
constexpr auto request(Ts &&...) -> t::request<Service, Ts...>;

template <typename Service, typename... Ts>
constexpr auto response(Ts &&...) -> t::response<Service, Ts...>;

template <typename Ref>
auto response_ref() -> t::response_ref<Ref> { return {}; }

template <typename... Ts>
auto error_response(Ts &&...) -> t::error_response<Ts...>;

template <typename T, typename Name>
auto param(Name) -> t::param<Name, T>;

template <typename Name, typename Expression>
auto ensure(Name, Expression) -> t::ensure<Name, Expression>;

template <typename NameT, typename... Ts>
auto object(NameT, Ts &&...) -> t::object<NameT, Ts...>;
//    typename std::enable_if<kvasir::mpl::call<kvasir::mpl::all<is_objectc>, Ts...>, t::object<NameT, Ts...>>::type;

template <typename Name, typename HostPrefix>
auto service(Name &&, HostPrefix &&) -> t::service<Name, HostPrefix>;

const t::any_other any_other;
const t::on_failure on_failure;

namespace literals {
template <typename CharT, CharT... String>
constexpr t::string<String...> operator""_s() noexcept {
    return {};
}
}  // namespace literals

using literals::operator""_s;
}  // namespace arp
