#pragma once

#include <string>
#include <memory>
#include <unordered_set>

struct AutoUpdateConfig {
    std::string md5jsonUrl;
    std::string resUrl;
    std::string remoteMd5JsonLocalDir;
    std::string versionKey;
    std::string localMd5JsonLocalDir;
    std::unordered_set<std::string> omittedLocalFiles;
};

extern std::unique_ptr<AutoUpdateConfig> g_autoUpateCfg;
