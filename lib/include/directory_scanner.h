#pragma once

#include <boost/filesystem.hpp>
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace boost::filesystem;

namespace bayan
{
    /**
     * @brief Reresents funtionality of directory scanning.
     */
    class DirectoryScanner
    {
    public:
        /**
         * @brief Represents file paths, grouped by file size.
         */
        using GroupedBySizeMap = std::unordered_map<size_t, std::unordered_set<std::string>>;

        /**
         * @brief Creates instance of @link DirectoryScanner::DirectoryScanner @endlink.
         *
         * @param dir_paths collection of paths to target directories.
         *
         * @param exclude_dirs collection of paths to directories that msut be excluded from the search.
         *
         * @param min_file_size_bytes minimum file size in bytes.
         */
        DirectoryScanner(const std::vector<std::string>& dir_paths, const std::vector<std::string>& exclude_dirs, size_t min_file_size_bytes = 1);

        DirectoryScanner(const DirectoryScanner&) = default;
        DirectoryScanner(DirectoryScanner&&) = default;

        DirectoryScanner& operator =(const DirectoryScanner&) = default;
        DirectoryScanner& operator =(DirectoryScanner&&) = default;

        /**
         * @brief Scans directories and returns collection of found file paths, groupded by file size.
         *
         * @param file_mask defines which file names would be included into search.
         *
         * @param is_recursive indicicates directory scanning level. True - recursive scanning, False - only top level scanning.
         *
         * @return file paths, grouped by file size.
         */
        GroupedBySizeMap scan(const std::string& file_mask = ".*", bool is_recursive = true);

    private:
        std::vector<std::string> m_dir_paths;
        std::vector<std::string> m_exclude_dirs;
        size_t m_min_file_size_bytes;

        GroupedBySizeMap recursive_scan(std::regex&&);

        GroupedBySizeMap top_level_scan(std::regex&&);

        void handle_file(const boost::filesystem::path&, const std::regex&, GroupedBySizeMap&);
    };
}

