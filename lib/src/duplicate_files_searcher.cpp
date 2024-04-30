#include "../include/duplicate_files_searcher.h"

#include <type_traits>
#include <unordered_map>
#include <utility>

using namespace bayan;

/**
 * @brief Creates instance of @link DuplicateFilesSearcher::DuplicateFilesSearcher @endlink.
 *
 * @param block_size content block size to be read from file.
 *
 * @param hash_algorithm hash algorithm type.
 *
 * @param min_file_size_bytes minimum file size in bytes.
 */
DuplicateFilesSearcher::DuplicateFilesSearcher(size_t block_size, HashAlgorithm hash_algorithm, size_t min_file_size_bytes)
    : m_block_size{block_size},
    m_min_file_size_bytes{min_file_size_bytes}
{
    switch (hash_algorithm)
    {
        case HashAlgorithm::MD5:
            m_hash = std::make_shared<MD5>();
            break;
        
        default:
            m_hash = std::make_shared<CRC32>();
            break;
    }
}

/**
 * @brief run search for duplicate files.
 *
 * @param dir_paths collection of paths to target directories.
 *
 * @param exclude_dir collection of paths to directories that msut be excluded from the search.
 *
 * @param file_mask defines which file names  would be included into search.
 *
 * @param is_recursive indicicates directory scanning level. True - recursive scanning, False - only top level scanning.
 *
 * @return grouped duplicates.
 */
DuplicateFilesSearcher::Duplicates DuplicateFilesSearcher::run(const std::vector<std::string>& dir_paths,
    const std::vector<std::string>& exclude_dirs, const std::string& file_mask, bool is_recursive)
{
    DirectoryScanner scanner(dir_paths, exclude_dirs, m_min_file_size_bytes);
    auto grouped_by_size = scanner.scan(file_mask, is_recursive);
    FileHashToPathBimap file_path_to_hash;

    for (auto& group : grouped_by_size)
    {
        if (group.second.size() < 2) { continue; }

        std::unordered_map<std::string, ComparableFileContent> file_content_cache(group.second.size());
        auto end = group.second.end();

        for (auto iter_left = group.second.begin(); iter_left != end; ++iter_left)
        {
            const std::string& file_path_left = *iter_left;
            auto& file_content_left = get_file_content(file_content_cache, file_path_left);

            std::unordered_set<std::string>::iterator it_r(iter_left);
            for (auto iter_right = ++it_r; iter_right != end; ++iter_right)
            {
                const std::string& file_path_right = *iter_right;
                auto& file_content_right = get_file_content(file_content_cache, file_path_right);

                if (file_content_left == file_content_right)
                {
                    replace_file_to_hash(file_path_to_hash, file_path_left, file_content_left);
                    replace_file_to_hash(file_path_to_hash, file_path_right, file_content_right);
                }
            }
        }
    }

    return build_duplicates(grouped_by_size, file_path_to_hash);
}

ComparableFileContent& DuplicateFilesSearcher::get_file_content(
        std::unordered_map<std::string, ComparableFileContent>& file_content_cache, const std::string& file_path) const
{
    if (!file_content_cache.contains(file_path))
    {
        file_content_cache.emplace(file_path, ComparableFileContent(file_path, m_block_size, m_hash));
    }
    return file_content_cache.at(file_path);
}

DuplicateFilesSearcher::Duplicates DuplicateFilesSearcher::build_duplicates(
        const bayan::DirectoryScanner::GroupedBySizeMap& grouped_by_size, const FileHashToPathBimap& file_path_to_hash)
{
    DuplicateFilesSearcher::Duplicates grouped_duplicates;
    grouped_duplicates.reserve(grouped_by_size.size());

    std::unordered_map<std::string, bool> processed_hashes;
    for (const auto& [file_path, hash] : file_path_to_hash)
    {
        if (processed_hashes.contains(hash))
        {
            continue;
        }

        auto duplicate_group = file_path_to_hash.by<FileHash>().equal_range(hash);
        processed_hashes.emplace(hash, true);

        std::unordered_set<std::string> s;
        for(auto iter = duplicate_group.first; iter != duplicate_group.second; ++iter)
        {
            s.insert(iter->second);
        }

        grouped_duplicates.push_back(std::move(s));
    }

    return grouped_duplicates;
}

void DuplicateFilesSearcher::replace_file_to_hash(
        FileHashToPathBimap& file_path_to_hash, const std::string& file_path, const ComparableFileContent& file_content)
{
    if (file_path_to_hash.left.count(file_path) != 0)
    {
        file_path_to_hash.left.erase(file_path);
    }
    file_path_to_hash.insert({ file_path, file_content.get_hash_from_already_read_content() });
}
