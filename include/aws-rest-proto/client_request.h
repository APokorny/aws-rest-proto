#pragma once
#include <string>
#include <aws-rest-proto/request_response_lang.h>
#include <aws-rest-proto/detail/client_request.h>
#include <aws-rest-proto/detail/string.h>
#include <aws-rest-proto/detail/callback_handler.h>
#include <aws-rest-proto/payload.h>
#include <aws-rest-proto/fields.h>
#include <aws-rest-proto/tuple.h>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <kvasir/mpl/algorithm/flatten.hpp>
#include <nlohmann/json.hpp>

namespace arp {

template <typename RequestType, typename... Ts>
std::unique_ptr<detail::client_request> async_request(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& stream, Ts&&... ts) {
    namespace ad = arp::detail;

    using service = detail::service<RequestType>;
    using errors = detail::errors<RequestType>;
    using fields = detail::fields<RequestType>;

    auto req_store = std::make_unique<detail::client_request>(stream);
    arp::params<Ts...> params(static_cast<Ts&&>(ts)...);

    req_store->setup_post(ad::c_str(t::string<'/'>, concat_t<ad::name<service>, t::string<'/'>, ad::name<RequestType>>{}),
                          ad::c_str(concat_t < ad::host<service>));
    req_store->req.body() = generate_payload(detail::payload<RequestType>{}, params).data();
    apply_fields(fields{}, req_store->req, params);
    req_store->finalize_post();
    req_store->write_and_read(get_failure_handler(params), [params](request_token::response_type& res) {
        auto payload = nlohmann::json::parse(res.body());
        using responses =
            kvasir::mpl::call<detail::unpack_arp<detail::filter_responses<kvasir::mpl::transform<detail::to_response>>>, RequestType>;
        if (error_str.size() == 0)
            ad::for_each_response(responses{}, params, payload);
    });

    return req_store;
}
}  // namespace arp
