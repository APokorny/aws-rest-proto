#pragma once

#include "aws-rest-proto/request_response_lang.h"
#include "aws-rest-proto/parameters.h"
#include "fmt/format.h"
#include <type_traits>

namespace arp {

template <typename Value>
typename std::enable_if<std::is_arithmetic<Value>::value>::type format_value(fmt::memory_buffer& buf, Value const& value) {
    format_to(buf, "{}", value);
}

template <typename Value>
typename std::enable_if<!std::is_arithmetic<Value>::value>::type format_value(fmt::memory_buffer& buf, Value const& value) {
    format_to(buf, R"("{}")", value);
}

template <typename Name, typename Value, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::param<Name, Value>, Params const& params) {
    format_to(buf, R"({}"{}":)", first_in_set ? "" : ",", detail::c_str(Name{}));
    format_value(buf, get<Name>(params));
    first_in_set = false;
    return 0;
}

template <typename Name, typename Value, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::ensure<Name, Value>, Params const&) {
    format_to(buf, R"({}"{}":)", first_in_set ? "" : ",", detail::c_str(Name{}));
    format_value(buf, detail::c_str(Value{}));
    first_in_set = false;
    return 0;
}

template <typename Name, typename... Elements, typename Params>
int apply_payload_element(fmt::memory_buffer& buf, bool& first_in_set, t::object<Name, Elements...>, Params const& params) {
    format_to(buf, R"({}"{}":{{)", first_in_set ? "" : ",", detail::c_str(Name{}));
    first_in_set = false;
    bool first_in_children = true;
    int foo[] = {apply_payload_element(buf, first_in_children, Elements{}, params)...};
    format_to(buf, "}}");
    return 0;
}

template <typename... Ts, typename... Params>
auto generate_payload(t::payload<Ts...> const& payload, params<Params...> const& params) {
    fmt::memory_buffer buf;
    format_to(buf, "{{");
    bool first_in_set = true;
    int foo[] = {apply_payload_element(buf, first_in_set, Ts{}, params)...};
    format_to(buf, "}}");
    return buf;
}
}  // namespace arp
