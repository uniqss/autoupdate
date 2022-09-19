#pragma once


#include <functional>

class HttpHelper {
   public:
    using wget_finish_cb =
        std::function<void(const char* url, const char* filepath, int code, std::size_t content_length)>;

    int wget(const char* url, const char* filepath, int timeout_seconds, wget_finish_cb finish_cb);
};
