#pragma once

#include <vector>
#include <string>

#include "fileversioninfo.h"

class DirUpdater {
   public:
    int DoUpdate(const FileVersionUpdateInfo& update, const std::string& url);
};
