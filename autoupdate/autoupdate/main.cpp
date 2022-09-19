
#include <iostream>
#include "file_version_queue.h"
#include "http_helper.h"

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
    if (!fq_remote.FromJsonFile(jsonFile)) {
        cout << "FromJsonFile error:" << endl;
        return -1;
    }
    fq_remote.Print();

    FileVersionQueue fq_local;
    fq_local.FromCurrDir();
    fq_local.Print();

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

    return 0;
}

int main() {
    const char* md5remote_url = "http://127.0.0.1/a/__md5.json";
    const char* md5remote = ".\\remote_md5.json";
    HttpHelper hhelper;
    hhelper.wget(md5remote_url, md5remote, -1, get_remote_md5_cb);

    return 0;
}
