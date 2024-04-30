#include "../include/directory_scanner.h"

#include <iostream>
#include <p_glob.h>

using namespace bayan;

/**
 * @brief Creates instance of @link DirectoryScanner::DirectoryScanner @endlink.
 *
 * @param dir_paths collection of paths to target directories.
 *
 * @param exclude_dirs collection of paths to directories that msut be excluded from the search.
 *
 * @param min_file_size_bytes minimum file size in bytes.
 */
DirectoryScanner::DirectoryScanner(const std::vector<std::string>& dir_paths,
    const std::vector<std::string>& exclude_dirs, size_t min_file_size_bytes)
    : m_dir_paths{dir_paths},
      m_exclude_dirs{exclude_dirs},
      m_min_file_size_bytes{min_file_size_bytes}
{}

/**
 * @brief Scans directories and returns collection of found file paths, groupded by file size.
 *
 * @param file_mask defines which file names would be included into search.
 *
 * @param is_recursive indicicates directory scanning level. True - recursive scanning, False - only top level scanning.
 *
 * @return file paths, grouped by file size.
 */
DirectoryScanner::GroupedBySizeMap DirectoryScanner::scan(const std::string& file_mask, bool is_recursive)
{
    try
    {
        auto file_mask_regex = pglob::compile_pattern(file_mask);
        return is_recursive
            ? recursive_scan(std::move(file_mask_regex))
            : top_level_scan(std::move(file_mask_regex));
    }
    catch(const filesystem_error& fex)
    {
        std::cerr << fex.what() << std::endl;
        throw;
    }
}

DirectoryScanner::GroupedBySizeMap DirectoryScanner::recursive_scan(std::regex&& file_mask_regex)
{
    GroupedBySizeMap groups;
    for (const auto& dir_path : m_dir_paths)
    {
        if (!exists(dir_path) || !is_directory(dir_path))
        {
            throw std::runtime_error("'" + dir_path + "'" + " is not a directory.");
        }

        recursive_directory_iterator dir_iterator(dir_path);
        for (const auto& fs_item : dir_iterator)
        {
            auto path = fs_item.path();

            if (is_directory(path))
            {
                auto exclude_dirs_end = m_exclude_dirs.end();
                if (auto it = std::find(m_exclude_dirs.begin(), exclude_dirs_end, path.string()); it != exclude_dirs_end)
                {
                    dir_iterator.disable_recursion_pending();
                }
                continue;
            }

            if (is_regular_file(path))
            {
                handle_file(path, file_mask_regex, groups);
                continue;
            }

            if (is_symlink(path))
            {
                // TODO: handle symlinks
            }
        }
    }
    return groups;
}

DirectoryScanner::GroupedBySizeMap DirectoryScanner::top_level_scan(std::regex&& file_mask_regex)
{
    GroupedBySizeMap groups;
    for (const auto& dir_path : m_dir_paths)
    {
        if (!exists(dir_path) || !is_directory(dir_path))
        {
            throw std::runtime_error("'" + dir_path + "'" + " is not a directory.");
        }

        directory_iterator dir_iterator(dir_path);
        for (const auto& fs_item : dir_iterator)
        {
            auto path = fs_item.path();

            if (is_regular_file(path))
            {
                handle_file(path, file_mask_regex, groups);
                continue;
            }

            if (is_symlink(path))
            {
                // TODO: handle symlinks
            }
        }
    }
    return groups;
}

void DirectoryScanner::handle_file(const boost::filesystem::path& path, const std::regex& file_mask_regex, GroupedBySizeMap& groups)
{
    const auto file_name = path.filename().string();
    std::smatch what;
    if (!std::regex_match(file_name, what, file_mask_regex))
    {
        return;
    }

    auto size = file_size(path);
    if (size < m_min_file_size_bytes)
    {
        return;
    }

    const auto& path_string = path.string();
    if (groups.contains(size))
    {
        auto& set = groups[size];
        set.insert(path_string);
    }
    else
    {
        groups[size] = { path_string };
    }
}
