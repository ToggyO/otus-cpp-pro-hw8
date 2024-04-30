#pragma once

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/range/iterator_range.hpp>
#include <string_view>

#include "../include/comparable_file_content.h"
#include "../include/directory_scanner.h"
#include "../include/hash_algorithm.h"
#include "../include/hashing.h"

namespace bayan
{
    namespace
    {
        struct FilePath {};
        struct FileHash {};

        using FileHashToPathBimap = boost::bimap<
            boost::bimaps::unordered_set_of<
                    boost::bimaps::tagged<std::string, FilePath>>,
            boost::bimaps::multiset_of<
                    boost::bimaps::tagged<std::string, FileHash>>>;
    }

    /**
     * @brief Represents functionality to search duplicate files.
     */
    class DuplicateFilesSearcher final
    {
    public:
        /**
         * @brief Grouped duplicates.
         */
        using Duplicates = std::vector<std::unordered_set<std::string>>;

        /**
         * @brief Creates instance of @link DuplicateFilesSearcher::DuplicateFilesSearcher @endlink.
         *
         * @param block_size content block size to be read from file.
         *
         * @param hash_algorithm hash algorithm type.
         *
         * @param min_file_size_bytes minimum file size in bytes.
         */
        DuplicateFilesSearcher(size_t block_size, HashAlgorithm hash_algorithm, size_t min_file_size_bytes = 1);

        DuplicateFilesSearcher(const DuplicateFilesSearcher&) = default;
        DuplicateFilesSearcher(DuplicateFilesSearcher&&) = default;

        /**
         * @brief run search for duplicate files.
         *
         * @param dir_paths collection of paths to target directories.
         *
         * @param exclude_dir collection of paths to directories that msut be excluded from the search.
         *
         * @param file_mask defines which file names would be included into search.
         *
         * @param is_recursive indicicates directory scanning level. True - recursive scanning, False - only top level scanning.
         *
         * @return grouped duplicates.
         */
        Duplicates run(const std::vector<std::string>& dir_paths, const std::vector<std::string>& exclude_dirs, const std::string& file_mask = ".*", bool is_recursive = true);

        DuplicateFilesSearcher& operator =(const DuplicateFilesSearcher&) = default;
        DuplicateFilesSearcher& operator =(DuplicateFilesSearcher&&) = default;

    private:
        size_t m_block_size;
        size_t m_min_file_size_bytes;
        std::shared_ptr<IHash> m_hash;

        ComparableFileContent& get_file_content(std::unordered_map<std::string, ComparableFileContent>& file_content_cache, const std::string& file_path) const;

        // [[nodiscard]] GroupedBySizeMap get_files_grouped_by_size(const std::vector<std::string>& dir_paths,
        //     const std::vector<std::string>& exclude_dirs, const std::string& file_mask, bool is_recursive = true);

        [[nodiscard]] static Duplicates build_duplicates(const bayan::DirectoryScanner::GroupedBySizeMap& grouped_by_size, const FileHashToPathBimap& file_path_to_hash);

        static void replace_file_to_hash(FileHashToPathBimap& file_path_to_hash, const std::string& file_path, const ComparableFileContent& file_content);
    };
}


