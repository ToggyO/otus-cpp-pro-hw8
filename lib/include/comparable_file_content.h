#pragma once

#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <fstream>
#include <list>
#include <memory>

#include "../include/hashing.h"

using namespace bayan::hashing;

namespace bayan
{
    /**
     * @brief Represents a file content, that can be compared with other same instance
     * by comparing fix sized blocks of hashed content.
     */
    class ComparableFileContent final
    {
    public:
        /**
         * @brief Creates instance of @link ComparableFileContent::ComparableFileContent @endlink.
         *
         * @param file_path path to file.
         *
         * @param readable_block_size content block size to be read from file.
         *
         * @param hash_ptr pointer to hashing object.
         */
        ComparableFileContent(const std::string& file_path, size_t readable_block_size, const std::shared_ptr<IHash>& hash_ptr);

        ComparableFileContent(const ComparableFileContent&) = delete;

        /**
         * @brief ComparableFileContent move ctor.
         */
        ComparableFileContent(ComparableFileContent&&) noexcept;

        /**
         * @brief Attepmts to retrieve a hash from the next fix sized block of file content.
         *
         * @param next_hash reference to hashed content.
         *
         * @return result indicates, whether the hash retrieving process was successful or not.
         */
        bool try_get_next_hash(std::string& next_hash);

        /**
         * @brief get_total_hash retrieves hash ffrom whole already read content.
         *
         * @return hashed string.
         */
        std::string get_hash_from_already_read_content() const;

        /**
         * @brief reset resets hash iterator.
         */
        void reset() noexcept;

        /**
         * @brief operator == compare two instances of @link ComparableFileContent @endlink.
         *
         * @param f1 first instance.
         *
         * @param f2 second instance.
         *
         * @return comporiosn result.
         */
        friend bool operator==(ComparableFileContent& f1, ComparableFileContent& f2);

        ComparableFileContent& operator =(const ComparableFileContent&) = delete;

        /**
         * @brief operator = ComparableFileContent move assign,ent operator.
         *
         * @return reference to assigned instance.
         */
        ComparableFileContent& operator =(ComparableFileContent&&) noexcept;

    private:
        std::string m_file_path;
        std::ifstream m_fs;
        size_t m_file_size;
        size_t m_block_size;

        std::list<std::string> m_cached_hashes;
        std::list<std::string>::iterator m_current_cached_position;
        const static std::list<std::string>::iterator m_default_iterator_position;

        std::weak_ptr<IHash> m_hash_ptr;

        bool try_get_from_fs(std::string& next_hash);
    };

    inline const std::list<std::string>::iterator ComparableFileContent::m_default_iterator_position = std::list<std::string>::iterator{};

    bool operator==(ComparableFileContent& f1, ComparableFileContent& f2);
}
