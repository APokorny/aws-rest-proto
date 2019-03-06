#pragma once
#include <aws-rest-proto/request_response_lang_fwd.h>
#include <kvasir/mpl/types/bool.hpp>
#include <kvasir/mpl/algorithm/find_if.hpp>
#include <kvasir/mpl/algorithm/filter.hpp>
#include <kvasir/mpl/sequence/front.hpp>

namespace arp {
namespace detail {
struct is_error {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};

    template <typename... Es>
    struct f<t::error_response<Es...>> : kvasir::mpl::bool_<true> {};
};
struct is_response {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};

    template <typename T>
    struct f<t::response_ref<T>> : kvasir::mpl::bool_<true> {};
    template <typename... Ts>
    struct f<t::response<Ts...>> : kvasir::mpl::bool_<true> {};
};

struct to_response {
    template <typename T>
    struct f_impl {
        using type = T;
    };

    template <typename T>
    struct f_impl<t::response_ref<T>> {
        using type = T;
    };
    template <typename... Ts>
    struct f_impl<t::response<Ts...>> {
        using type = t::response<Ts...>;
    };
    template <typename T>
    using f = typename f_impl<T>::type;
};

struct is_payload {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};

    template <typename... Es>
    struct f<t::payload<Es...>> : kvasir::mpl::bool_<true> {};
};

struct is_string {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};

    template <char... Cs>
    struct f<t::string<Cs...>> : kvasir::mpl::bool_<true> {};
};

struct valid_payload {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};
    template <typename... Es>
    struct f<t::object<Es...>> : kvasir::mpl::bool_<true> {};
    template <typename Name, typename Value>
    struct f<t::param<Name, Value>> : kvasir::mpl::bool_<true> {};
    template <typename Name, typename Value>
    struct f<t::ensure<Name, Value>> : kvasir::mpl::bool_<true> {};
};

struct valid_request {
    template <typename T, typename D = void>
    struct f : kvasir::mpl::bool_<false> {};
    template <typename... Es, typename D>
    struct f<t::payload<Es...>, D> : kvasir::mpl::bool_<true> {};
    template <typename... Es>
    struct f<t::error_response<Es...>, void> : kvasir::mpl::bool_<true> {};
    template <typename... Es>
    struct f<t::response<Es...>, void> : kvasir::mpl::bool_<true> {};
    template <typename Ref>
    struct f<t::response_ref<Ref>, void> : kvasir::mpl::bool_<true> {};
    template <typename T>
    struct f<T, typename std::enable_if<std::is_same<std::decay_t<T>, t::on_failure>::value>::type> : kvasir::mpl::bool_<true> {};
};

struct valid_error {
    template <typename T, typename D = void>
    struct f : kvasir::mpl::bool_<false> {};
    template <char... Cs>
    struct f<t::string<Cs...>, void> : kvasir::mpl::bool_<true> {};
    template <typename T>
    struct f<T, typename std::enable_if<std::is_same<std::decay_t<T>, t::any_other>::value>::type> : kvasir::mpl::bool_<true> {};
};

namespace impl {
template <typename T>
struct host;
template <typename Name, typename Host>
struct host<arp::t::service<Name, Host>> {
    using type = Host;
};
template <typename T>
struct name;
template <typename Name, typename Host>
struct name<arp::t::service<Name, Host>> {
    using type = Name;
};
template <typename Service, typename Name, typename Host>
struct name<arp::t::response<Service, Name, Host>> {
    using type = Name;
};
template <typename Service, typename Name, typename... Ts>
struct name<arp::t::request<Service, Name, Ts...>> {
    using type = Name;
};
template <typename T>
struct service;
template <typename Service, typename Name, typename... Ts>
struct service<t::request<Service, Name, Ts...>> {
    using type = Service;
};
template <typename Service, typename Name, typename... Ts>
struct service<t::response<Service, Name, Ts...>> {
    using type = Service;
};

template <typename C, typename L, typename... Ts>
struct unpack;
template <typename C, typename S, typename T, typename... Ls, typename... Ts>
struct unpack<C, t::request<S, T, Ls...>, Ts...> {
    using type = typename kvasir::mpl::dcall<C, sizeof...(Ls)>::template f<Ts..., Ls...>;
};

template <typename C, typename S, typename T, typename... Ls, typename... Ts>
struct unpack<C, t::response_ref<t::response<S, T, Ls...>>, Ts...> {
    using type = typename kvasir::mpl::dcall<C, sizeof...(Ls)>::template f<Ts..., Ls...>;
};

template <typename C, typename S, typename T, typename... Ls, typename... Ts>
struct unpack<C, t::response<S, T, Ls...>, Ts...> {
    using type = typename kvasir::mpl::dcall<C, sizeof...(Ls)>::template f<Ts..., Ls...>;
};

}  // namespace impl
/// \brief Turns composite nodes inside the AWS Request&resPonse language into variadic types
template <typename C>
struct unpack_arp {
    template <typename... Ls>
    using f = typename impl::unpack<C, Ls...>::type;
};

template <typename C = kvasir::mpl::identity>
using filter_errors = kvasir::mpl::find_if<detail::is_error, C>;
template <typename C = kvasir::mpl::identity>
using filter_payload = kvasir::mpl::find_if<detail::is_payload, C>;
template <typename C = kvasir::mpl::identity>
using filter_responses = kvasir::mpl::find_if<detail::is_response, C>;

template <typename T>
using host = typename impl::host<T>::type;
template <typename T>
using name = typename impl::name<T>::type;
template <typename T>
using service = typename impl::service<T>::type;
template <typename T>
using payload = typename kvasir::mpl::call<unpack_arp<filter_payload<kvasir::mpl::front<>>>, T>;
template <typename T>
using errors = typename kvasir::mpl::call<unpack_arp<filter_errors<kvasir::mpl::front<>>>, T>;
template <typename T>
using responses = typename kvasir::mpl::call<unpack_arp<filter_responses<kvasir::mpl::transform<detail::to_response>>>, T>;

namespace impl {
template <typename T>
struct flatten_object {
    using type = kvasir::mpl::list<T>;
};

template <typename N, typename... Ts>
struct flatten_object<t::object<N, Ts...>> {
    using type = typename kvasir::mpl::detail::join_select<kvasir::mpl::detail::select_join_size(
        sizeof...(Ts))>::template f<kvasir::mpl::list, typename flatten_object<Ts>::type...>::type;
};
}  // namespace impl
/// \brief converts a tree or list of lists into one list containing the contents of all
/// children \effects \requires example call<flatten<>,list<void>,list<list<int>,char>,bool>
/// resolves to list<void,int,char,bool>.
template <typename C = kvasir::mpl::listify>
struct flatten_object {
    template <typename... Ts>
    using f = typename kvasir::mpl::detail::join_select<kvasir::mpl::detail::select_join_size(
        sizeof...(Ts))>::template f<C::template f, typename impl::flatten_object<Ts>::type...>::type;
};

template <typename C, typename T>
using test = typename C::template f<T>;

}  // namespace detail
}  // namespace arp
