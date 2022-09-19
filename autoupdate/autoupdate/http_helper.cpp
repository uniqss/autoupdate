#include "http_helper.h"

#include "HTTPRequest.hpp"

#include <iostream>
#include <fstream>


int HttpHelper::wget(const char* url, const char* filepath, int timeout_seconds, wget_finish_cb finish_cb) {
    try {
        std::string uri = url;
        std::string method = "GET";
        std::string arguments;
        std::string output = filepath;
        auto protocol = http::InternetProtocol::V4;

        http::Request request{uri, protocol};

        const auto response = request.send(
            method, arguments,
            {{"Content-Type", "application/x-www-form-urlencoded"}, {"User-Agent", "runscope/0.1"}, {"Accept", "*/*"}},
            std::chrono::seconds(timeout_seconds));

        // std::cout << response.status.reason << '\n';

        std::size_t bodylen = 0;

        if (response.status.code == http::Status::Ok) {
            if (!output.empty()) {
                bodylen = response.body.size();
                std::ofstream outfile{output, std::ofstream::binary};
                outfile.write(reinterpret_cast<const char*>(response.body.data()),
                              static_cast<std::streamsize>(response.body.size()));
            } else
                std::cout << std::string{response.body.begin(), response.body.end()} << '\n';
        }

        finish_cb(url, filepath, response.status.code, bodylen);

    } catch (const http::RequestError& e) {
        std::cerr << "Request error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const http::ResponseError& e) {
        std::cerr << "Response error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
