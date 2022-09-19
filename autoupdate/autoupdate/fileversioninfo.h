#pragma once

#include <string>
#include <iostream>
#include <vector>

struct FileVersionInfo {
    std::string folder;
    std::string fileName;
    std::string md5;
};

std::ostream& operator<<(std::ostream& os, const FileVersionInfo& info);

struct FileVersionUpdateInfo {
    std::vector<FileVersionInfo> add;
    std::vector<FileVersionInfo> update;
    std::vector<FileVersionInfo> del;
};
