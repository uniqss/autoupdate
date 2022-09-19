#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "fileversioninfo.h"

class FileVersionQueue {
   public:
    std::vector<FileVersionInfo> filesQueue;

   public:
    bool FromJsonFile(const char* jsonFilePath);
    void Print();
    int FromCurrDir();

    int Diff(const FileVersionQueue& remote, FileVersionUpdateInfo& retUpdate);
};
