#include "autoupdate_config.h"

std::unique_ptr<AutoUpdateConfig> g_autoUpateCfg =
    std::unique_ptr<AutoUpdateConfig>(new AutoUpdateConfig{"http://127.0.0.1/md5/md5.json",
                                                           "http://127.0.0.1/res/",
                                                           ".\\remote_md5.json",
                                                           "__version__",
                                                           ".\\localmd5.json",
                                                           {".\\autoupdate.exe", ".\\autoupdate.pdb"}});
;
