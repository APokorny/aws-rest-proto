#pragma once

#include "aws-rest-proto/request_response_lang.h"

namespace arp {
template <typename Name, typename Request, typename... Params>
void apply_fields(t::field<Name>, Request const& req, params<Params...> const& params) {
    req.set(c_str(Name{}), get<Name>(params));
}

template <typename... Fields, typename Request, typename... Params>
void apply_fields(kvasir::mpl::list<Fields...>, Request const& req, params<Params...> const& params) {
    auto trick = {apply_field(Fields{}, req, params)...};
    (void)trick;
}
}  // namespace arp
