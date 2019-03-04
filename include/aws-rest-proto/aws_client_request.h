#pragma once
#include <string>
#include <aws-rest-proto/request_response_lang.h>
#include <aws-rest-proto/detail/aws_client_request.h>
#include <aws-rest-proto/detail/string.h>
#include <aws-rest-proto/detail/callback_handler.h>
#include <aws-rest-proto/payload.h>
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

using request_token = detail::aws_client_request;

template <typename RequestType, typename... Ts>
std::unique_ptr<request_token> async_aws_request(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& stream, std::string const& region,
                                                 Ts&&... ts) {
    namespace ad = arp::detail;

    using service = detail::service<RequestType>;
    using errors = detail::errors<RequestType>;

    auto req_store = std::make_unique<request_token>(stream);
    arp::params<Ts...> params(static_cast<Ts&&>(ts)...);

    req_store->setup_post(ad::c_str(concat_t<ad::name<service>, t::string<'.'>, ad::name<RequestType>>{}),
                          (ad::c_str(concat_t<ad::host<service>, t::string<'.'>>{}) + region + ".amazonaws.com").c_str());
    req_store->req.body() = generate_payload(detail::payload<RequestType>{}, params).data();
    req_store->finalize_post();
    req_store->write_and_read(get_failure_handler(params), [params](request_token::response_type& res) {
        auto const error_str = res["x-amz-errortype"];
        auto payload = nlohmann::json::parse(res.body());
        using responses =
            kvasir::mpl::call<detail::unpack_arp<detail::filter_responses<kvasir::mpl::transform<detail::to_response>>>, RequestType>;
        if (error_str.size() == 0)
            ad::for_each_response(responses{}, params, payload);
        else
            ad::for_each_error(errors{}, params, error_str, payload);
    });

    return req_store;
}
}  // namespace arp