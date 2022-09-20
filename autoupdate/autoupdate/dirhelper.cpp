#include "dirhelper.h"

#include <iostream>
#include <filesystem>
using std::cout;
using std::endl;
namespace fs = std::filesystem;

int readFileList(const char* basePath, std::vector<std::pair<bool, std::string>>* vecRetNoPath,
                 std::vector<std::pair<bool, std::string>>* vecRetWithPath) {
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(basePath)) {
        // Is it a file / directory?
        bool isNormalFile = fs::is_regular_file(entry);
        bool isDirectory = fs::is_directory(entry);
        auto path = entry.path();
        // Path: my-directory/test.txt
        std::string pathString = path.string();
        if (vecRetWithPath) vecRetWithPath->push_back({isDirectory, pathString});
        // Filename: test.txt
        std::string filenameString = path.filename().string();
        if (vecRetNoPath) vecRetNoPath->push_back({isDirectory, filenameString});
        // Extension: txt
        std::string extensionString = path.extension().string();
        // NOTE: You can also "cout << path" directly
    }

    return 0;
}

bool isPathAbsWin(const std::string& path) {
    return (path.size() >= 2 && path[1] == ':');
}

int makeDir(const char* path) {
    return std::filesystem::create_directories(path) ? 0 : -1;
}

int makeDir(const std::string& path) {
    return makeDir(path.c_str());
}

int removeFile(const std::string& path) {
    return std::filesystem::remove(path) ? 0 : -1;
}
