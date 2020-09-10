#pragma once

#include "aws-rest-proto/request_response_lang.h"
#include "aws-rest-proto/parameters.h"
#include "kvasir/mpl/sequence/push_front.hpp"
#include "kvasir/mpl/sequence/push_back.hpp"
#include "kvasir/mpl/algorithm/fold_left.hpp"
#include "fmt/format.h"
#include <type_traits>
#include <iostream>

namespace arp
{
#if 0
namespace str
{
using qm          = decltype("\""_s);
using comma       = decltype(","_s);
using colon       = decltype(":"_s);
using open_p      = decltype("{{"_s);
using close_p     = decltype("}}"_s);
using placeholder = decltype("{}"_s);
}  // namespace str

struct list_start
{
};

template <typename T>
struct interleave_element
{
    template <typename L, typename R>
    struct f_impl
    {
        using type = concat_t<L, T, R>;
    };
    template <typename R>
    struct f_impl<list_start, R>
    {
        using type = R;
    };
    template <typename Left, typename Right>
    using f = typename f_impl<Left, Right>::type;
};

struct to_json
{
    template <typename T>
    struct f_impl
    {
        using type = T;
    };
    template <typename N, typename V>
    struct f_impl<t::param<N, V>>
    {
        using type = concat_t<str::qm, N, str::qm, str::colon, str::placeholder>;
    };

    template <typename N, typename V>
    struct f_impl<t::ensure<N, V>>
    {
        using type = concat_t<str::qm, N, str::qm, str::colon, str::qm, V, str::qm>;
    };

    template <typename N, typename... Ts>
    struct f_impl<t::object<N, Ts...>>
    {
        using type = concat_t<                                                                    //
            str::qm, N, str::qm, str::colon,                                                      //
            kvasir::mpl::call<                                                                    //
                kvasir::mpl::transform<                                                           //
                    to_json,                                                                      //
                    kvasir::mpl::push_front<                                                      //
                        list_start,                                                               //
                        kvasir::mpl::fold_left<                                                   //
                            interleave_element<str::comma>,                                       //
                            kvasir::mpl::push_front<                                              //
                                str::open_p,                                                      //
                                kvasir::mpl::push_back<str::close_p, kvasir::mpl::cfe<concat_t>>  //
                                >>>>,
                Ts...>>;
        ;
    };
    template <typename T>
    using f = typename f_impl<T>::type;
};

template <typename T>
struct debug_print;
template <typename... Ts, typename... Params>
auto generate_payload(t::payload<Ts...> const& payload, params<Params...> const& params)
{
    fmt::memory_buffer buf;
    using str         = kvasir::mpl::call<                                               //
        kvasir::mpl::transform<                                                  //
            to_json,                                                             //
            kvasir::mpl::push_front<                                             //
                list_start,                                                      //
                kvasir::mpl::fold_left<interleave_element<str::comma>,           //
                                       kvasir::mpl::push_front<                  //
                                           str::open_p,                          //
                                           kvasir::mpl::push_back<str::close_p>  //
                                           >>>>,
        Ts...>;
    auto payload_spec = concat_list(str{});
    format_to(buf, c_str(payload_spec), params...);

    return buf;
}
#else

template <typename Value>
typename std::enable_if<std::is_arithmetic<Value>::value>::type format_value(fmt::memory_buffer& buf, Value const& value)
{
    format_to(buf, "{}", value);
}

template <typename Value>
typename std::enable_if<!std::is_arithmetic<Value>::value>::type format_value(fmt::memory_buffer& buf, Value const& value)
{
    format_to(buf, R"("{}")", value);
}

template <typename Name, typename Value, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::param<Name, Value>, Params const& params)
{
    format_to(buf, R"({}"{}":)", first_in_set ? "" : ",", detail::c_str(Name{}));
    format_value(buf, get<Name>(params));
    first_in_set = false;
    return 0;
}

template <typename Name, typename Value, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::ensure<Name, Value>, Params const&)
{
    format_to(buf, R"({}"{}":)", first_in_set ? "" : ",", detail::c_str(Name{}));
    format_value(buf, detail::c_str(Value{}));
    first_in_set = false;
    return 0;
}

template <typename Name, typename... Elements, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::object<Name, Elements...>, Params const& params)
{
    format_to(buf, R"({}"{}":{{)", first_in_set ? "" : ",", detail::c_str(Name{}));
    first_in_set           = false;
    bool first_in_children = true;
    auto foo               = {apply_payload_element(buf, first_in_children, Elements{}, params)...};
    format_to(buf, "}}");
    return 0;
}

template <typename... Ts, typename... Params>
auto generate_payload(t::payload<Ts...> const& payload, params<Params...> const& params)
{
    fmt::memory_buffer buf;
    format_to(buf, "{{");
    bool first_in_set   = true;
    auto unroll_payload = {apply_payload_element(buf, first_in_set, Ts{}, params)...};
    format_to(buf, "}}");
    return buf;
}
#endif
}  // namespace arp
