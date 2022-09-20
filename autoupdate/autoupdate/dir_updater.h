#pragma once

#include <vector>
#include <string>

#include "fileversioninfo.h"

class DirUpdater {
    FileVersionUpdateInfo updateInfo;
    std::string resUrlPrefix;

   public:
    int DoUpdate(const FileVersionUpdateInfo& update, const std::string& resUrlPrefix);

   private:
    int _update();
};
