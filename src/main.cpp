#include <iostream>

#include <boost/program_options.hpp>

#include "duplicate_files_searcher.h"

int main(int argc, char** argv)
{
    boost::program_options::options_description options("General options");

    options.add_options()
        ("help,H", "help message")
        ("dir,D", boost::program_options::value<std::vector<std::string>>(), "Target dir")
        ("exclude_dir,E", boost::program_options::value<std::vector<std::string>>(), "Exclude dir")
        ("recursive,R", boost::program_options::value<bool>()->default_value(true), "Level of scan: 0 - top level only, 1 - recursive")
        ("min_file_size,F", boost::program_options::value<size_t>()->default_value(1), "Min file size in bytes")
        ("file_mask,M", boost::program_options::value<std::string>()->default_value(".*"), "File mask regex")
        ("block_size,S", boost::program_options::value<size_t>(), "Block size to read")
        ("hash_algorithm,H", boost::program_options::value<size_t>()->default_value(0), "Hash algorithm");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);

    if (!vm.count("dir"))
    {
        throw new std::runtime_error("'dir' is required");
    }
    auto dirs = vm["dir"].as<std::vector<std::string>>();

    if (!vm.count("block_size"))
    {
        throw new std::runtime_error("'block_size' is required");
    }
    auto block_size = vm["block_size"].as<size_t>();

    bool recursive = vm["recursive"].as<bool>();
    size_t min_file_size = vm["min_file_size"].as<size_t>();
    std::string file_mask =  vm["file_mask"].as<std::string>();
    auto hash_algorithm = (bayan::hashing::HashAlgorithm)vm["hash_algorithm"].as<size_t>();
    auto exclude_dirs = !vm.count("exclude_dir")
        ? std::vector<std::string>()
        : vm["exclude_dir"].as<std::vector<std::string>>();

    bayan::DuplicateFilesSearcher searcher(block_size, hash_algorithm, min_file_size);
    try
    {
        auto duplicates = searcher.run(dirs, exclude_dirs, file_mask, recursive);
        for (const auto& group : duplicates)
        {
            for (const auto& path : group)
            {
                std::cout << path << std::endl;
            }
            std::cout << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Bayan working process aborted: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
