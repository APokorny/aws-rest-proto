#include "aws-rest-proto/aws_client_request.h"
#include <boost/beast/version.hpp>
#include "digest.h"
#include "encode.h"

namespace ba = boost::asio;
namespace ssl = ba::ssl;
namespace bad = ba::detail;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ad = arp::detail;

ad::aws_client_request::aws_client_request(ssl::stream<ba::ip::tcp::socket>& stream) : stream(stream) {}

void ad::aws_client_request::setup_post(char const* request, char const* host) {
    req.method(http::verb::post);
    req.target("/");
    req.set("Host", host);
    req.set("Accept", "*/*");
    req.set("Connection ", "keep-alive");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set("Content-Type", "application/x-amz-json-1.1");
    req.set("X-Amz-Target", request);
    req.set("x-amz-api-version", "2016-04-18");
}

void ad::aws_client_request::finalize_post() {
    req.set("X-Amz-Content-Sha256", base64_encode(digest_message(req.body())));
    req.content_length(req.body().size());
}

void ad::aws_client_request::write_and_read(std::function<void(beast::error_code)>&& fail_handler,
                                            std::function<void(response_type&)>&& result_handler) {
    error_handler = std::move(fail_handler);
    response_handler = std::move(result_handler);
    http::async_write(stream, req, std::bind(&aws_client_request::on_write, this, std::placeholders::_1, std::placeholders::_2));
}

void ad::aws_client_request::on_write(boost::beast::error_code ec, std::size_t transfered) {
    if (ec) return error_handler(ec);
    http::async_read(stream, buffer, res, std::bind(&aws_client_request::on_read, this, std::placeholders::_1, std::placeholders::_2));
}
void ad::aws_client_request::on_read(boost::beast::error_code ec, std::size_t transfered) {
    if (ec) return error_handler(ec);
    response_handler(res);
}
