#pragma once
#include "http_server.hpp"

HttpServer::HttpServer(const std::string& host, int port, const std::string& directory) 
    : host_(host)
    , port_(port)
    , directory_(directory)
{}

void HttpServer::start() {
    
}
