#include "file_version_queue.h"

#include "yyjson.h"

#include <iostream>
using std::cout;
using std::endl;

#include <filesystem>
namespace fs = std::filesystem;

#include "umd5.h"

#include <unordered_map>

static bool iterate_json_obj(yyjson_val *obj, std::string path, std::vector<FileVersionInfo> &fqueue) {
    yyjson_obj_iter iter;
    if (!yyjson_obj_iter_init(obj, &iter)) return false;

    yyjson_val *key, *val;
    while ((key = yyjson_obj_iter_next(&iter))) {
        val = yyjson_obj_iter_get_val(key);
        auto type = yyjson_get_type(val);
        auto keystr = yyjson_get_str(key);
        if (type == YYJSON_TYPE_OBJ) {
            if (!iterate_json_obj(val, path + keystr + "\\", fqueue)) {
                return false;
            }
            // yyjson_obj_iter_get_val(key);
            // printf("%s: %s\n", keystr, yyjson_get_type_desc(val));
        } else {
            // printf("%s => %s\n", keystr, yyjson_get_str(val));
            FileVersionInfo finfo;
            finfo.fileName = keystr;
            finfo.folder = path;
            finfo.md5 = yyjson_get_str(val);
            fqueue.emplace_back(finfo);
        }
    }
    return true;
}

bool FileVersionQueue::FromJsonFile(const char *jsonFilePath) {
    bool ret = false;

    do {
        yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
        yyjson_read_err err;
        yyjson_doc *doc = yyjson_read_file(jsonFilePath, flg, NULL, &err);

        if (doc != nullptr) {
            yyjson_val *obj = yyjson_doc_get_root(doc);
            if (obj == nullptr) {
                ret = false;
                break;
            }
            ret = iterate_json_obj(obj, ".\\", this->filesQueue);
            if (!ret) break;
        } else {
            printf("read error (%u): %s at position: %zd\n", err.code, err.msg, err.pos);
            ret = false;
            break;
        }

        yyjson_doc_free(doc);
    } while (false);


    return ret;
}

void FileVersionQueue::Print() {
    std::string delimiter = "\t";
    for (auto finfo : filesQueue) {
        cout << finfo << endl;
    }
    cout << endl;
    cout << endl;
}


static int readFileList(const char *basePath, std::vector<std::string> *vecRetNoPath,
                        std::vector<std::string> *vecRetWithPath) {
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(basePath)) {
        // Is it a file / directory?
        bool isNormalFile = fs::is_regular_file(entry);
        bool isDirectory = fs::is_directory(entry);
        auto path = entry.path();
        // Path: my-directory/test.txt
        std::string pathString = path.string();
        if (vecRetWithPath) vecRetWithPath->push_back(pathString);
        // Filename: test.txt
        std::string filenameString = path.filename().string();
        if (vecRetNoPath) vecRetNoPath->push_back(filenameString);
        // Extension: txt
        std::string extensionString = path.extension().string();
        // NOTE: You can also "cout << path" directly
    }

    return 0;
}

static bool RecursiveDirAndSubDir(std::string basePath, std::vector<FileVersionInfo> &fqueue) {
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(basePath)) {
        auto path = entry.path();
        std::string filenameString = path.filename().string();

        // Is it a file / directory?
        bool isDirectory = fs::is_directory(entry);
        if (isDirectory) {
            return RecursiveDirAndSubDir(basePath + filenameString + "\\", fqueue);
        }

        bool isNormalFile = fs::is_regular_file(entry);
        if (!isNormalFile) return false;

        // Path: my-directory/test.txt
        std::string pathString = path.string();
        // Extension: txt
        // std::string extensionString = path.extension().string();
        // NOTE: You can also "cout << path" directly

        FileVersionInfo info;
        // Filename: test.txt
        info.fileName = path.filename().string();
        info.folder = basePath;
        if (!MD5_file(pathString.c_str(), 32, info.md5)) {
            return false;
        }
        fqueue.emplace_back(info);
    }

    return true;
}

int FileVersionQueue::FromCurrDir() {
    this->filesQueue.clear();

    const char *basePath = ".\\";
    RecursiveDirAndSubDir(basePath, this->filesQueue);
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
        } else if (it_remote->second->md5 != it.md5) {
            retUpdate.update.emplace_back(*it_remote->second);
            remote_dict.erase(fullpath);
        }
    }

    for (auto it : remote_dict) {
        retUpdate.add.emplace_back(*it.second);
    }

    return 0;
}
