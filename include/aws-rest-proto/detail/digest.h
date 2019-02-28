#pragma once
#include "cryptopp/sha.h"
#include "nonstd/span.hpp"

namespace arp {
namespace detail {

template <typename CharT>
std::vector<unsigned char> digest_message(nonstd::span<CharT> const& data);
extern template std::vector<unsigned char> digest_message(nonstd::span<const char> const&);
extern template std::vector<unsigned char> digest_message(nonstd::span<const unsigned char> const&);
inline std::vector<unsigned char> digest_message(std::string const& data) {
    return digest_message(nonstd::span<unsigned char const>(reinterpret_cast<unsigned char const*>(data.data()), data.size()));
}
std::string base64_encode(nonstd::span<const unsigned char> const& binary);

}  // namespace detail
}  // namespace arp
