#include "dir_updater.h"
#include "http_helper.h"
#include "dirhelper.h"

#include <algorithm>

int dir_update_cb(const char* url, const char* filepath, int retcode, size_t content_len) {
    if (retcode != 200) {
        std::cout << "update failed. url:" << url << " filepath:" << filepath << " retcode:" << retcode << std::endl;
    }
    return 0;
}

int DirUpdater::DoUpdate(const FileVersionUpdateInfo& update, const std::string& resUrlPrefix) {
    updateInfo.clear();
    updateInfo = update;

    this->resUrlPrefix = resUrlPrefix;

    return _update();
}

static std::string joinHttpUrl(const std::string& urlPrefix, const std::string& folder, const std::string& file) {
    std::string ret = urlPrefix;
    ret += folder.substr(2);
    ret += file;
    std::for_each(ret.begin(), ret.end(), [](char& c) {
        if (c == '\\') c = '/';
    });
    return ret;
}

int DirUpdater::_update() {
    int ret = 0;
    HttpHelper http_helper;
    for (auto it : updateInfo.add) {
        ret = makeDir(it.folder);

        std::string wget_url = joinHttpUrl(resUrlPrefix, it.folder, it.fileName);
        std::string local_path = it.folder + it.fileName;
        http_helper.wget(wget_url.c_str(), local_path.c_str(), -1, dir_update_cb);
    }

    for (auto it : updateInfo.update) {
        std::string wget_url = joinHttpUrl(resUrlPrefix, it.folder, it.fileName);
        std::string local_path = it.folder + it.fileName;
        http_helper.wget(wget_url.c_str(), local_path.c_str(), -1, dir_update_cb);
    }

    for (auto it : updateInfo.del) {
        std::string filepath = it.folder + it.fileName;
        removeFile(filepath);
    }

    return ret;
}
