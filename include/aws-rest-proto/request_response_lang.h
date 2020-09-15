#pragma once
#include <aws-rest-proto/detail/string.h>
#include <aws-rest-proto/request_response_lang_fwd.h>
#include <aws-rest-proto/detail/item.h>
#include <aws-rest-proto/detail/request_response_lang.h>
#include <kvasir/mpl/algorithm/all.hpp>
#include <kvasir/mpl/functional/call.hpp>
#include <type_traits>

namespace arp
{
namespace t
{
template <typename T>
struct assignable
{
    template <typename OtherT>
    constexpr auto operator=(OtherT &&other) const -> detail::item<T, std::decay_t<OtherT>>
    {
        return detail::item<T, std::decay_t<OtherT>>(static_cast<OtherT &&>(other));
    }
};
template <typename Service, typename Name, typename... Ts>
struct response : assignable<response<Service, Name, Ts...>>
{
    using assignable<response<Service, Name, Ts...>>::operator=;
};
template <typename T>
struct response_ref : assignable<response_ref<T>>
{
    using assignable<response_ref<T>>::operator=;
};

template <typename T>
struct error_ref : assignable<error_ref<T>>
{
    using assignable<error_ref<T>>::operator=;
};
template <typename Service, typename Name, typename... Ts>
struct request
{
};
template <typename Name, typename Value>
struct param
{
};
template <typename Name, typename Expression>
struct ensure
{
};
template <typename Name, typename... Ts>
struct object
{
};
struct any_other : assignable<any_other>
{
    using assignable<any_other>::operator=;
};
struct on_failure : assignable<on_failure>
{
    using assignable<on_failure>::operator=;
};
template <typename... Ts>
struct payload
{
};
template <typename... Ts>
struct error_response
{
};
template <typename Name, typename HostPrefix>
struct service
{
};
template <typename Name>
struct field
{
};
}  // namespace t

template <typename... Ts>
constexpr auto payload(Ts &&...)
    -> std::enable_if_t<kvasir::mpl::call<kvasir::mpl::all<detail::valid_payload>, Ts...>::value, t::payload<Ts...>>;

template <typename Service, typename Name, typename... Ts>
constexpr auto request(Name &&, Ts &&...) -> std::enable_if_t<detail::test<detail::is_string, Name>::value &&
                                                                  kvasir::mpl::call<kvasir::mpl::all<detail::valid_request>, Ts...>::value,
                                                              t::request<Service, Name, Ts...>>;

template <typename Service, typename Name, typename... Ts>
constexpr auto response(Name &&, Ts &&...) -> std::enable_if_t<detail::test<detail::is_string, Name>::value &&
                                                                   kvasir::mpl::call<kvasir::mpl::all<detail::valid_request>, Ts...>::value,
                                                               t::response<Service, Name, Ts...>>;

template <typename Ref>
auto response_ref() -> std::enable_if_t<detail::test<detail::is_response, std::decay_t<Ref>>::value, t::response_ref<Ref>>
{
    return {};
}

template <typename Ref>
auto error_ref() -> std::enable_if_t<detail::test<detail::is_string, std::decay_t<Ref>>::value, t::error_ref<Ref>>
{
    return {};
}

template <typename... Ts>
auto error_response(Ts &&...)
    -> std::enable_if_t<kvasir::mpl::call<kvasir::mpl::all<detail::valid_error>, Ts...>::value, t::error_response<Ts...>>;

template <typename T, typename Name>
auto param(Name) -> t::param<Name, T>;

template <typename Name, typename Expression>
auto ensure(Name, Expression) -> t::ensure<Name, Expression>;

template <typename NameT, typename... Ts>
auto object(NameT, Ts &&...) -> std::enable_if_t<detail::test<detail::is_string, NameT>::value &&
                                                     kvasir::mpl::call<kvasir::mpl::all<detail::valid_payload>, Ts...>::value,
                                                 t::object<NameT, Ts...>>;

template <typename Name, typename HostPrefix>
auto service(Name &&, HostPrefix &&) -> t::service<Name, HostPrefix>;

template <typename Name>
auto field(Name &&) -> t::field<Name>;

const t::any_other  any_other;
const t::on_failure on_failure;

namespace literals
{
template <typename CharT, CharT... String>
constexpr t::string<String...> operator""_s() noexcept
{
    return {};
}
}  // namespace literals

using literals::operator""_s;
}  // namespace arp
