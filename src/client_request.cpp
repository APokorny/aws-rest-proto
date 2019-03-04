#include "aws-rest-proto/detail/client_request.h"
#include <boost/beast/version.hpp>
#include "encode.h"

namespace ba = boost::asio;
namespace ssl = ba::ssl;
namespace bad = ba::detail;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ad = arp::detail;

ad::client_request::client_request(ssl::stream<ba::ip::tcp::socket>& stream) : stream(stream) {}

void ad::client_request::setup_post(char const* request, char const* host) {
    req.method(http::verb::post);
    req.target(request);
    req.set("Host", host);
    req.set("Accept", "application/json");
    req.set("Connection ", "keep-alive");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set("Content-Type", "application/json");
}

void ad::client_request::finalize_post() {
    req.content_length(req.body().size());
}

void ad::client_request::write_and_read(std::function<void(beast::error_code)>&& fail_handler,
                                        std::function<void(response_type&)>&& result_handler) {
    error_handler = std::move(fail_handler);
    response_handler = std::move(result_handler);
    http::async_write(stream, req, std::bind(&aws_client_request::on_write, this, std::placeholders::_1, std::placeholders::_2));
}

void ad::client_request::on_write(boost::beast::error_code ec, std::size_t transfered) {
    if (ec) return error_handler(ec);
    http::async_read(stream, buffer, res, std::bind(&aws_client_request::on_read, this, std::placeholders::_1, std::placeholders::_2));
}
void ad::client_request::on_read(boost::beast::error_code ec, std::size_t transfered) {
    if (ec) return error_handler(ec);
    response_handler(res);
}
