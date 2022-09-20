#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>
#include <string>


int readFileList(const char* basePath, std::vector<std::pair<bool, std::string>>* vecRetNoPath,
                 std::vector<std::pair<bool, std::string>>* vecRetWithPath);
bool isPathAbsWin(const std::string& path);
int makeDir(const char* path);
int makeDir(const std::string& path);
int removeFile(const std::string& path);
