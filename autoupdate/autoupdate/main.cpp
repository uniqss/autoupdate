
#include <iostream>
#include "file_version_queue.h"
#include "http_helper.h"
#include "dir_updater.h"

#include "autoupdate_config.h"

#include <algorithm>
using std::cout;
using std::endl;

int get_remote_md5_cb(const char* url, const char* filepath, int retcode, size_t content_len) {
    if (retcode != 200) {
        cout << "get remote json failed." << endl;
        return -1;
    }
    FileVersionQueue fq_remote;
    const char* jsonFile = filepath;
    std::string remoteVersion;
    if (!fq_remote.FromJsonFile(jsonFile, remoteVersion)) {
        cout << "FromJsonFile error:" << endl;
        return -1;
    }
    fq_remote.Print();

    FileVersionQueue fq_local;
    fq_local.FromCurrDir();
    fq_local.Print();
    fq_local.SaveJson(g_autoUpateCfg->localMd5JsonLocalDir, remoteVersion);

    FileVersionUpdateInfo update_info;
    fq_local.Diff(fq_remote, update_info);
    auto print = [](const FileVersionInfo& info) { cout << info << endl; };
    cout << endl << "Add" << endl;
    std::for_each(update_info.add.begin(), update_info.add.end(), print);
    cout << endl << "Update" << endl;
    std::for_each(update_info.update.begin(), update_info.update.end(), print);
    cout << endl << "Del" << endl;
    std::for_each(update_info.del.begin(), update_info.del.end(), print);
    cout << endl << endl;

    DirUpdater updater;
    std::string resUrlPrefix = g_autoUpateCfg->resUrl + remoteVersion + "/";
    updater.DoUpdate(update_info, resUrlPrefix);

    return 0;
}

void help() {
    cout << "currently supported mode:" << endl;
    cout << "directly run with no args" << endl;
    cout << "xxx.exe genlocalmd5 123     => 123 is the new version" << endl;
    cout << endl;
}

int genlocalmd5(const std::string& version) {
    int ret = 0;
    FileVersionQueue fq_local;
    ret = fq_local.FromCurrDir();
    if (ret != 0) return ret;
    fq_local.Print();
    ret = fq_local.SaveJson(".\\md5.json", version);
    if (ret != 0) return ret;

    return 0;
}

int main(int argc, const char** argv) {
    if (argc > 1) {
        if (argc < 3) {
            help();
            return -1;
        }
        std::string mode = argv[1];
        std::string version = argv[2];
        if (mode == "genlocalmd5") {
            return genlocalmd5(version);
        }
        help();
        return -1;
    }

    const char* md5remote_url = g_autoUpateCfg->md5jsonUrl.c_str();
    const char* md5remote = g_autoUpateCfg->remoteMd5JsonLocalDir.c_str();
    HttpHelper hhelper;
    hhelper.wget(md5remote_url, md5remote, -1, get_remote_md5_cb);

    return 0;
}
