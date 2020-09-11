#pragma once
#include <string>
#include <aws-rest-proto/request_response_lang.h>
#include <aws-rest-proto/detail/string.h>
#include <aws-rest-proto/tuple.h>
#include <aws-rest-proto/parameters.h>
#include <boost/asio/ssl/error.hpp>
#include <kvasir/mpl/algorithm/flatten.hpp>
#include <nlohmann/json.hpp>

namespace arp {
namespace detail {
template <typename... Ts>
using void_t = void;
template <typename Result, typename = void>
struct is_invocable_impl : std::false_type {};

template <typename Result>
struct is_invocable_impl<Result, void_t<typename Result::type>> : std::true_type {};

template <typename Fn, typename... ArgTypes>
struct is_invocable : is_invocable_impl<std::result_of<Fn && (ArgTypes && ...)>>::type {};

template <typename Name, typename Value, typename Params>
bool get_payload_element(nlohmann::json const& data, t::param<Name, Value>, Params& params) {
    auto pos = data.find(detail::c_str(Name{}));
    if (pos == data.end()) return false;
    pos->get_to(get<Name>(params));
    return true;
}

template <typename Name, char... Cs, typename Params>
bool get_payload_element(nlohmann::json const& data, t::ensure<Name, t::string<Cs...>>, Params& params) {
    auto pos = data.find(c_str(Name{}));
    if (pos == data.end() || pos->template get<std::string>() != c_str(t::string<Cs...>{})) return false;
    return true;
}

template <typename Name, typename... Elements, typename Params>
bool get_payload_element(nlohmann::json const& data, t::object<Name, Elements...>, Params& params) {
    auto pos = data.find(c_str(Name{}));
    if (pos == data.end()) return false;
    bool ret = true;
    bool const foo[] = {(ret = ret && get_payload_element(*pos, Elements{}, params))...};
    return ret;
}

template <typename F, typename... Items>
typename std::enable_if<is_invocable<F, arp::params<Items...> const&>::value>::type execute(F&& f,
                                                                                            arp::params<Items...> const& parsed_payload,
                                                                                            nlohmann::json const& /*unused*/) {
    return f(parsed_payload);
}

template <typename F, typename... Items>
typename std::enable_if<is_invocable<F, Items...>::value>::type execute(F&& f, arp::params<Items...> const& parsed_payload,
                                                                        nlohmann::json const& /*unused*/) {
    return f(static_cast<Items const&>(parsed_payload)...);
}
template <typename F, typename... Items>
typename std::enable_if<is_invocable<F, value_type<Items>...>::value>::type execute(F&& f, arp::params<Items...> const& parsed_payload,
                                                                                    nlohmann::json const& /*unused*/) {
    return f(item_get(static_cast<Items const&>(parsed_payload))...);
}
template <typename F, typename... Items>
typename std::enable_if<is_invocable<F, nlohmann::json>::value>::type execute(F&& f, arp::params<Items...> const& /*unused*/,
                                                                              nlohmann::json const& payload) {
    return f(payload);
}

template <typename F>
decltype(std::declval<F>()(std::declval<boost::string_view>(), std::declval<nlohmann::json>()))
/*typename std::enable_if<is_invocable<F, boost::string_view const&, nlohmann::json const&>::value>::type*/
execute(F&& f, boost::string_view const& error_str, nlohmann::json const& payload) {
    return f(error_str, payload);
}

struct only_param {
    template <typename T>
    struct f : kvasir::mpl::bool_<false> {};
    template <typename K, typename V>
    struct f<t::param<K, V>> : kvasir::mpl::bool_<true> {};
};

struct param_to_item {
    template <typename T>
    struct f_impl;
    template <typename K, typename V>
    struct f_impl<t::param<K, V>> {
        using type = detail::item<K, V>;
    };
    template <typename T>
    using f = typename f_impl<T>::type;
};

template <typename... Ts, typename VariableParameters>
bool execute_payload(arp::t::payload<Ts...>, nlohmann::json const& payload, VariableParameters& params) {
    bool parse_payload = true;
    const bool foo[] = {(parse_payload = parse_payload && get_payload_element(payload, Ts{}, params))...};
    return parse_payload;
}

template <typename S, typename Name, typename... Ts, typename Parameters>
constexpr typename std::enable_if<(has_key<t::response_ref<t::response<S, Name, Ts...>>, Parameters>::value ||
                                   has_key<t::response<S, Name, Ts...>, Parameters>::value),
                                  bool>::type
try_execute_response(arp::t::response<S, Name, Ts...>, Parameters const& parameters, nlohmann::json const& payload) {
    using key = t::response_ref<t::response<S, Name, Ts...>>;
    using expected_payload = kvasir::mpl::call<filter_payload<kvasir::mpl::front<>>, Ts...>;
    using variable_elements =
        kvasir::mpl::call<filter_payload<kvasir::mpl::front<kvasir::mpl::unpack<flatten_object<
                              kvasir::mpl::filter<only_param, kvasir::mpl::transform<param_to_item, kvasir::mpl::cfe<arp::params>>>>>>>,
                          Ts...>;
    variable_elements elements;
    bool parse_payload = execute_payload(expected_payload{}, payload, elements);
    if (parse_payload) {
        execute(get<key>(parameters), elements, payload);
        return false;
    }
    return true;
}

template <typename S, typename Name, typename... Ts, typename Parameters>
constexpr typename std::enable_if<!(has_key<t::response_ref<t::response<S, Name, Ts...>>, Parameters>::value ||
                                    has_key<t::response<S, Name, Ts...>, Parameters>::value),
                                  bool>::type
try_execute_response(arp::t::response<S, Name, Ts...>, Parameters const&, nlohmann::json const&) {
    return true;
}

template <typename... Responses, typename Parameters>
constexpr void for_each_response(kvasir::mpl::list<Responses...>, Parameters const& parameters, nlohmann::json const& payload) {
    bool cont = true;
    bool execute_until_first_match[] = {cont = cont && try_execute_response(Responses{}, parameters, payload)...};
    (void)execute_until_first_match;
}

template <typename ErrorName, typename Parameters>
typename std::enable_if<has_key<ErrorName, Parameters>::value, bool>::type try_execute_error(Parameters const& paras,
                                                                                             boost::string_view const& error_str,
                                                                                             nlohmann::json const& payload) {
    if (error_str == c_str(ErrorName{})) {
        execute(get<ErrorName>(paras), error_str, payload);
        return false;
    }
    return true;
}

template <typename ErrorName, typename Parameters>
typename std::enable_if<!has_key<ErrorName, Parameters>::value, bool>::type try_execute_error(Parameters const& paras,
                                                                                              boost::string_view const& error_str,
                                                                                              nlohmann::json const& payload) {
    return true;
}

template <typename... Errors, typename Parameters>
void for_each_error(t::error_response<Errors...>, Parameters const& paras, boost::string_view const& error_str,
                    nlohmann::json const& payload) {
    bool cont = true;
    bool execute_until_first_match[] = {cont = cont && try_execute_error<Errors>(paras, error_str, payload)...};
    if (cont) execute(get<t::any_other>(paras), error_str, payload);
}

template <typename... Ts>
typename std::enable_if<has_key<arp::t::on_failure, params<Ts...>>::value, std::function<void(boost::beast::error_code)>>::type
get_failure_handler(params<Ts...> const& p) {
    return get<arp::t::on_failure>(p);
}

template <typename... Ts>
typename std::enable_if<!has_key<arp::t::on_failure, params<Ts...>>::value, std::function<void(boost::beast::error_code)>>::type
get_failure_handler(params<Ts...> const& p) {
    return [](boost::beast::error_code) {};
}
}  // namespace detail
}  // namespace arp
