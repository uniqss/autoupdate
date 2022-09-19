#include "fileversioninfo.h"


std::ostream& operator<<(std::ostream& os, const FileVersionInfo& info) {
    std::string delimiter = "\t";
    os << "fileName:" << info.fileName << delimiter;
    os << "folder:" << info.folder << delimiter;
    os << "md5:" << info.md5;
    return os;
}
