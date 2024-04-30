#include "../include/hashing.h"

#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

using boost::uuids::detail::md5;

std::string bayan::hashing::MD5::get_hash(const std::string& input)
{
    md5 hash;
    md5::digest_type digest;
    hash.process_bytes(input.data(), input.size());
    hash.get_digest(digest);

    const auto intDigest = reinterpret_cast<const int*>(&digest);
    std::string result;
    boost::algorithm::hex(intDigest, intDigest + (sizeof(md5::digest_type)/sizeof(int)), std::back_inserter(result));
    return result;
}

std::string bayan::hashing::CRC32::get_hash(const std::string& input)
{
    boost::crc_32_type result;
    result.process_bytes(input.data(), input.length());
    return std::to_string(result.checksum());
}