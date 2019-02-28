#include <string>
#include <vector>
#include <nonstd/span.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include "aws-rest-proto/detail/digest.h"

namespace bai = boost::archive::iterators;

template <typename CharT>
std::vector<unsigned char> arp::detail::digest_message(nonstd::span<CharT> const& data) {
    std::vector<unsigned char> digest(32);
    CryptoPP::SHA256 hash;
    hash.CalculateDigest(digest.data(), reinterpret_cast<const unsigned char*>(data.data()), data.size());
    return digest;
}

template std::vector<unsigned char> arp::detail::digest_message(nonstd::span<const char> const&);
template std::vector<unsigned char> arp::detail::digest_message(nonstd::span<const unsigned char> const&);

std::string arp::detail::base64_encode(nonstd::span<const unsigned char> const& binary) {
    using vit = nonstd::span<const unsigned char>::const_iterator;
    using it = bai::base64_from_binary<bai::transform_width<vit, 6, 8>>;
    auto ret = std::string(it(binary.begin()), it(binary.end()));
    char const* padding_tok[] = {"", "==", "="};
    ret += padding_tok[binary.size() % 3];
    return ret;
}
