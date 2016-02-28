#pragma once
#include <string>

class HttpServer {
private:
    std::string host_;
    int port_;
    std::string directory_;

public:
    HttpServer(const std::string& host, int port, const std::string& directory);
    void start();
};
