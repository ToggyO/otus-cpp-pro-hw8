#pragma once

#include <string>

namespace bayan::hashing
{
    /**
     * @brief Interface represents functionality to get hash from input string.
     */
    struct IHash
    {
        /**
         * @brief get_hash gets hash from input string.
         *
         * @return hashed string.
         */
        virtual std::string get_hash(const std::string&) = 0;
    };

    /**
     * @brief The MD5 hash algorithm.
     */
    struct MD5 final : IHash
    {
        /**
        * @copydoc IHash::get_hash(const std::string&)
        *
        * @brief gets hash from input string.
        */
        std::string get_hash(const std::string&) override;
    };


    /**
     * @brief The CRC32 hash algorithm.
     */
    struct CRC32 final : IHash
    {
        /**
        * @copydoc IHash::get_hash(const std::string&)
        *
        * @brief gets hash from input string.
        */
        std::string get_hash(const std::string&) override;
    };
}
