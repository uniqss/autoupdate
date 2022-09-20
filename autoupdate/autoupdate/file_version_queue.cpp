#include "file_version_queue.h"

#include <iostream>
using std::cout;
using std::endl;

#include <filesystem>
namespace fs = std::filesystem;

#include "umd5.h"

#include <unordered_map>

#include "nlohmann/json.hpp"
#include <fstream>
using json = nlohmann::json;

#include "autoupdate_config.h"

#include "dirhelper.h"

const std::string DELIMITER = "\\";

void TrimFolderAndFilePath(const std::string &folderAndFile, std::string &folder, std::string &file) {
    auto pos = folderAndFile.find_last_of(DELIMITER);
    if (pos == std::string::npos) {
        folder = "." + DELIMITER;
        file = folderAndFile;
    } else {
        folder = folderAndFile.substr(0, pos + DELIMITER.size());
        file = folderAndFile.substr(pos + DELIMITER.size());
    }
}

bool FileVersionQueue::FromJsonFile(const char *jsonFilePath, std::string &version) {
    std::ifstream f(jsonFilePath);
    json data = json::parse(f);
    cout << data << endl;
    for (json::iterator it = data.begin(); it != data.end(); ++it) {
        // cout << it.key() << "\t" << it.value() << endl;

        auto key = it.key();
        auto value = it.value();

        if (key == g_autoUpateCfg->versionKey) {
            version = value;
            continue;
        }

        if (g_autoUpateCfg->omittedLocalFiles.count(key) > 0) continue;

        FileVersionInfo finfo;
        std::string folderAndFile = key;

        TrimFolderAndFilePath(folderAndFile, finfo.folder, finfo.fileName);

        finfo.md5 = value;
        filesQueue.emplace_back(finfo);
    }

    return true;
}

void FileVersionQueue::Print() {
    for (auto finfo : filesQueue) {
        cout << finfo << endl;
    }
    cout << endl;
    cout << endl;
}

int FileVersionQueue::FromCurrDir() {
    this->filesQueue.clear();

    std::string basePath = "." + DELIMITER;

    std::vector<std::pair<bool, std::string>> vecRetWithPath;
    readFileList(basePath.c_str(), nullptr, &vecRetWithPath);

    for (auto it : vecRetWithPath) {
        if (!it.first) {
            const std::string &pathString = it.second;

            if (g_autoUpateCfg->omittedLocalFiles.count(pathString) > 0) continue;

            FileVersionInfo info;
            TrimFolderAndFilePath(pathString, info.folder, info.fileName);

            if (!MD5_file(pathString.c_str(), 32, info.md5)) {
                cout << "MD5_file failed. pathString:" << pathString << endl;
                return false;
            }
            this->filesQueue.emplace_back(info);
        }
    }

    return 0;
}

int FileVersionQueue::Diff(const FileVersionQueue &remote, FileVersionUpdateInfo &retUpdate) {
    std::unordered_map<std::string, const FileVersionInfo *> remote_dict;
    for (const auto &it : remote.filesQueue) {
        std::string fullpath = it.folder + it.fileName;
        remote_dict[fullpath] = &it;
    }

    for (const auto &it : this->filesQueue) {
        std::string fullpath = it.folder + it.fileName;
        auto it_remote = remote_dict.find(fullpath);
        if (it_remote == remote_dict.end()) {
            retUpdate.del.emplace_back(it);
            continue;
        } else {
            if (it_remote->second->md5 != it.md5) {
                retUpdate.update.emplace_back(*it_remote->second);
            }
            remote_dict.erase(fullpath);
        }
    }

    for (auto it : remote_dict) {
        retUpdate.add.emplace_back(*it.second);
    }

    return 0;
}

int FileVersionQueue::SaveJson(const std::string &filePath, const std::string &version) {
    int ret = 0;

    do {
        std::ofstream ofs(filePath.c_str());
        json data;
        data[g_autoUpateCfg->versionKey] = version;
        for (auto it : this->filesQueue) {
            data[it.folder + it.fileName] = it.md5;
        }
        ofs << std::setw(4) << data << std::endl;
        ofs.close();
    } while (false);

    return 0;
}
