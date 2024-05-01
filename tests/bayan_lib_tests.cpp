#include <gtest/gtest.h>

#include <unordered_map>

#include "config.h"
#include "duplicate_files_searcher.h"

template <class Collection1, class Collection2>
bool collections_are_equivalent(Collection1 left, Collection2 right)
{
    auto begin_left = left.begin();
    auto end_left = left.end();
    auto begin_right = right.begin();
    auto end_right = right.end();

    auto size_left = std::distance(begin_left, end_left);
    auto size_right = std::distance(begin_right, end_right);
    if (size_left != size_right)
    {
        return false;
    }

    std::unordered_map<std::string, bool> existing;

    for (auto it1 = begin_left; it1 != end_left; ++it1)
    {
        existing[*it1] = true;
    }

    for (auto it2 = begin_right; it2 != end_right; ++it2)
    {
        if (!existing.contains(*it2))
        {
            return false;
        }
    }

    return true;
}

// TODO: add gmock assertions
TEST(Bayan, RecursiveTest) {
    std::string root = get_test_project_root();

    std::vector<std::string> dir_paths { root + "/dir" };
    std::vector<std::string> exclude_dirs { root + "/dir/dir_to_exclude" };
    bool is_recursive = true;
    size_t block_size = 5;
    std::string file_mask = "*.*";
    bayan::hashing::HashAlgorithm hash_algorithm = bayan::hashing::HashAlgorithm::MD5;

    bayan::DuplicateFilesSearcher d(block_size, hash_algorithm);
    auto duplicates = d.run(dir_paths, exclude_dirs, file_mask, is_recursive);

    EXPECT_EQ(duplicates.size(), 4);

    std::vector<std::string> actual;
    actual.reserve(9);
    for (const auto &set : duplicates)
    {
        for (const auto& path : set)
        {
            actual.push_back(std::move(path));
        }
    }

    std::vector<std::string> expected
    {
        root + "/dir/dir1/dir1.1/file1.1.1.txt",
        root + "/dir/dir1/dir1.1/file1.1.2.txt",
        root + "/dir/dir1/file1.1.txt",
        root + "/dir/dir1/afile1.2.txt",
        root + "/dir/dir2/file2.1.log",
        root + "/dir/dir2/file2.1.txt",
        root + "/dir/dir2/file2.2.log",
        root + "/dir/dir3/file3.1.txt",
        root + "/dir/dir3/file3.2.txt",
    };

    EXPECT_TRUE(collections_are_equivalent(actual, expected));
}

TEST(Bayan, TopLevelTest) {
    std::string root = get_test_project_root();

    std::vector<std::string> dir_paths { root + "/dir/dir1" };
    std::vector<std::string> exclude_dirs;
    bool is_recursive = false;
    size_t block_size = 5;
    std::string file_mask = "*.*";
    bayan::hashing::HashAlgorithm hash_algorithm = bayan::hashing::HashAlgorithm::MD5;

    bayan::DuplicateFilesSearcher d(block_size, hash_algorithm);
    auto duplicates = d.run(dir_paths, exclude_dirs, file_mask, is_recursive);

    EXPECT_EQ(duplicates.size(), 1);

    EXPECT_EQ(duplicates[0],
      std::unordered_set<std::string>({
        root + "/dir/dir1/file1.1.txt",
        root + "/dir/dir1/afile1.2.txt",
      }));
}
