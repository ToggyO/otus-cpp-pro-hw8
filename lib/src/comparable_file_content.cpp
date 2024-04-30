#include "../include/comparable_file_content.h"

#include "boost/filesystem.hpp"
#include <sstream>

using namespace bayan;

/**
 * @brief Creates instance of @link ComparableFileContent::ComparableFileContent @endlink.
 *
 * @param file_path path to file.
 *
 * @param readable_block_size content block size to be read from file.
 *
 * @param hash_ptr pointer to hashing object.
 */
ComparableFileContent::ComparableFileContent(
    const std::string& file_path, size_t readable_block_size, const std::shared_ptr<IHash>& hash_ptr)
    : m_file_path{file_path},
    m_fs{std::ifstream(file_path, std::ios::binary)},
    m_file_size{boost::filesystem::file_size(file_path)},
    m_block_size{readable_block_size},
    m_cached_hashes{},
    m_current_cached_position{m_default_iterator_position},
    m_hash_ptr{hash_ptr}
{
    if (!m_fs.is_open())
    {
        throw std::runtime_error("Can't open file: '" + file_path + '\'' + '\n');
    }
}

/**
 * @brief ComparableFileContent move ctor.
 */
ComparableFileContent::ComparableFileContent(ComparableFileContent&& other) noexcept
    : m_file_path{std::move(other.m_file_path)},
    m_fs{std::move(other.m_fs)},
    m_file_size{std::move(other.m_file_size)},
    m_block_size{std::move(other.m_block_size)},
    m_cached_hashes{std::move(other.m_cached_hashes)},
    m_current_cached_position{std::move(other.m_current_cached_position)},
    m_hash_ptr{std::move(other.m_hash_ptr)}
{
    other.m_file_size = 0;
    other.m_block_size = 0;
    // ВОПРОС: верно ли я реализовал перемещение shared_ptr поля m_hash_ptr
}

/**
 * @brief Attepmts to retrieve a hash from the next fix sized block of file content.
 *
 * @param next_hash reference to hashed content.
 *
 * @return result indicates, whether the hash retrieving process was successful or not.
 */
bool ComparableFileContent::try_get_next_hash(std::string& next_hash)
{
    if (m_cached_hashes.empty() || m_current_cached_position == m_cached_hashes.end() || m_current_cached_position == m_default_iterator_position)
    {
        return try_get_from_fs(next_hash);
    }

    if (m_current_cached_position == m_default_iterator_position)
    {
        m_current_cached_position = m_cached_hashes.begin();
    }

    next_hash = *m_current_cached_position;
    m_current_cached_position++;
    return true;
}

/**
 * @brief get_total_hash get hash from whole already read content.
 * 
 * @return hashed string.
 */
std::string ComparableFileContent::get_hash_from_already_read_content() const
{
    auto hasher = m_hash_ptr.lock();
    if (!hasher)
    {
        throw std::runtime_error("No shared_ptr is locked!");
    }

    std::stringstream ss;
    for (const auto& hash : m_cached_hashes)
    {
        ss << hash;
    }

    return hasher->get_hash(ss.str());
}


/**
 * @brief reset resets hash iterator.
 */
void ComparableFileContent::reset() noexcept
{
    m_current_cached_position = m_cached_hashes.begin();
}

bool ComparableFileContent::try_get_from_fs(std::string& next_hash)
{
    if (!m_fs.is_open())
    {
        return false;
    }

    std::string buffer;
    buffer.resize(m_block_size);
    m_fs.read(&buffer[0], m_block_size);

    auto hasher = m_hash_ptr.lock();
    if (!hasher)
    {
        throw std::runtime_error("No shared_ptr is locked!");
    }

    m_cached_hashes.push_back(hasher->get_hash(buffer));
    next_hash = std::move(hasher->get_hash(buffer));

    if (m_fs.peek() == EOF)
    {
        m_fs.close();
    }

    return true;
}

/**
 * @brief operator = ComparableFileContent move assign,ent operator.
 *
 * @return reference to assigned instance.
 */
ComparableFileContent& ComparableFileContent::operator =(ComparableFileContent&& other) noexcept
{
    if (this == &other) { return *this; }

    m_file_path = std::move(other.m_file_path);
    m_fs = std::move(other.m_fs);
    m_file_size = std::move(other.m_file_size);
    m_block_size = std::move(other.m_block_size);
    m_cached_hashes = std::move(other.m_cached_hashes);
    m_current_cached_position = std::move(other.m_current_cached_position);
    m_hash_ptr = std::move(other.m_hash_ptr);

    other.m_file_size = 0;
    other.m_block_size = 0;

    return *this;
}


/**
 * @brief operator == compare two instances of @link ComparableFileContent @endlink.
 *
 * @param f1 first instance.
 *
 * @param f2 second instance.
 *
 * @return comporiosn result.
 */
bool bayan::operator==(ComparableFileContent& f1, ComparableFileContent& f2)
{
    if (f1.m_file_size != f2.m_file_size)
    {
        return false;
    }

    bool is_equal = true;

    std::string c1;
    std::string c2;

    while (f1.try_get_next_hash(c1) && f2.try_get_next_hash(c2))
    {
        if (c1 != c2)
        {
            is_equal = false;
            break;
        }
    }

    f1.reset();
    f2.reset();
    return is_equal;
}
