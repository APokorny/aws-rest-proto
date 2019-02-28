#pragma once
#include <string>
#include <vector>
#include "nonstd/span.hpp"

namespace arp {
std::vector<unsigned char> digest_message(nonstd::span<const char> const& data);
std::vector<unsigned char> digest_message(nonstd::span<const unsigned char> const& data);
std::string base64_encode(nonstd::span<const unsigned char> const& binary);
inline std::vector<unsigned char> digest_message(std::string const& data) {
    return digest_message(nonstd::span<unsigned char const>(reinterpret_cast<unsigned char const*>(data.data()), data.size()));
}

}
