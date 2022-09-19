#include "dir_updater.h"
#include "http_helper.h"
#include "dirhelper.h"

int dir_update_cb(const char* url, const char* filepath, int retcode, size_t content_len) {
    if (retcode != 200) {
        std::cout << "update failed. url:" << url << " filepath:" << filepath << std::endl;
    }
    return 0;
}

int DirUpdater::DoUpdate(const FileVersionUpdateInfo& update, const std::string& url) {
    int ret = 0;
    HttpHelper http_helper;
    for (auto it : update.add) {
        ret = makeDir(it.folder);
        if (ret != 0) {
            return ret;
        }
        std::string wget_url = url + it.fileName;
        std::string local_path = it.folder + it.fileName;
        http_helper.wget(wget_url.c_str(), local_path.c_str(), -1, dir_update_cb);
    }

    for (auto it : update.update) {
        std::string wget_url = url + it.fileName;
        std::string local_path = it.folder + it.fileName;
        http_helper.wget(wget_url.c_str(), local_path.c_str(), -1, dir_update_cb);
    }

    for (auto it : update.del) {
        std::string filepath = it.folder + it.fileName;
        removeFile(filepath);
    }

    return ret;
}
