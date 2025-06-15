// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "utils.hpp"
#include "utils_c.h"

#include <chrono>

namespace ob_smpl {
char waitForKeyPressed(uint32_t timeout_ms) {
    return ob_smpl_wait_for_key_press(timeout_ms);
}

uint64_t getNowTimesMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int getInputOption() {
    char inputOption = ob_smpl::waitForKeyPressed();
    if(inputOption == ESC_KEY) {
        return -1;
    }
    return inputOption - '0';
}

std::string getFormatTimestamp() {
    auto    currentTime = std::chrono::system_clock::now();
    auto    time_t_time = std::chrono::system_clock::to_time_t(currentTime);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &time_t_time);
#else
    localtime_r(&time_t_time, &tm);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d%H%M%S");
    return ss.str();
}

int mkDirs(const char *muldir) {
    size_t    i;
    size_t len;
    char   str[512];
    int    error = 0;
#ifdef _WIN32
    strncpy_s(str, muldir, 512);
    len = strlen(str);
    for(i = 0; i < len; i++) {
        if(str[i] == '/' || str[i] == '\\') {
            str[i] = '\0';
            if(_access(str, 0) != 0) {
                error = _mkdir(str);
                if(error != 0) {
                    return error;
                }
            }
            str[i] = '/';
        }
    }
    if(len > 0 && _access(str, 0) != 0) {
        error = _mkdir(str);
    }
    return error;
#else
    strncpy(str, muldir, 512);
    len = strlen(str);
    umask(0000);
    for(i = 0; i < len; i++) {
        if(i > 0 && str[i] == '/') {
            str[i] = '\0';
            if(access(str, 0) != 0) {
                error = mkdir(str, 0777);
                if(error != 0) {
                    return error;
                }
            }
            str[i] = '/';
        }
    }
    if(len > 0 && access(str, 0) != 0) {
        error = mkdir(str, 0777);
    }
    return error;
#endif
}
}  // namespace ob_smpl
