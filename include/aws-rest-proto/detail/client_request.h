#include <boost/asio/ssl/error.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "aws-rest-proto/detail/digest.h"

namespace arp {
namespace detail {
struct client_request {
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& stream;
    boost::beast::flat_buffer buffer;
    using response_type = boost::beast::http::response<boost::beast::http::string_body>;
    boost::beast::http::request<boost::beast::http::string_body> req;
    response_type res;
    std::function<void(boost::beast::error_code)> error_handler;
    std::function<void(response_type&)> response_handler;

    client_request(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& stream);
    void setup_post(char const* request, char const* host);
    void finalize_post();
    void write_and_read(std::function<void(boost::beast::error_code)>&& error_handler,
                        std::function<void(response_type&)>&& result_handler);
private:
    void on_write(boost::beast::error_code ec, std::size_t transfered);
    void on_read(boost::beast::error_code ec, std::size_t transfered);
};
}  // namespace detail
}  // namespace arp
