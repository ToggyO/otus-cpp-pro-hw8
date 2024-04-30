#include <gtest/gtest.h>

#include "config.h"
#include "duplicate_files_searcher.h"

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

    EXPECT_EQ(duplicates[0],
      std::unordered_set<std::string>({
          root + "/dir/dir1/dir1.1/file1.1.1.txt",
          root + "/dir/dir1/dir1.1/file1.1.2.txt",
      }));

    EXPECT_EQ(duplicates[1],
      std::unordered_set<std::string>({
        root + "/dir/dir1/file1.1.txt",
        root + "/dir/dir1/afile1.2.txt",
      }));

    EXPECT_EQ(duplicates[2],
      std::unordered_set<std::string>({
        root + "/dir/dir2/file2.1.log",
        root + "/dir/dir2/file2.1.txt",
        root + "/dir/dir2/file2.2.log"
      }));

    EXPECT_EQ(duplicates[3],
      std::unordered_set<std::string>({
        root + "/dir/dir3/file3.1.txt",
        root + "/dir/dir3/file3.2.txt",
      }));
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
