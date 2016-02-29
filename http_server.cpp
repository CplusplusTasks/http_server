#pragma once
#include "http_server.hpp"
#include <iostream>
#include <exception>

#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>

namespace ip = boost::asio::ip;

HttpServer::HttpServer(const std::string& host, unsigned short port, const std::string& directory) 
    : host_(host)
    , port_(port)
    , directory_(directory)
    , acceptor_(io_service_, ip::tcp::endpoint(ip::address::from_string(host), port))
    , log_("/home/eugene/university/3 course/stepic/multithreading_c++/http_server/log")
{
    acceptor_.set_option(ip::tcp::socket::reuse_address(true));
}

namespace asio = boost::asio;

void HttpServer::run() {
    std::function<void()> task;
    while (true) {
        {
            std::unique_lock<std::mutex> lk(m_);
            cv_.wait(lk, 
                [&]() -> bool {
                    return !q_.empty(); 
                }
            );
            task = q_.front();
            q_.pop();
        }
        task();
    }
}

void HttpServer::readHandler(Client* client, const boost::system::error_code & err, std::size_t read_bytes) {
    using namespace boost::asio;
    using std::placeholders::_1;
    using std::placeholders::_2;

    using namespace std;
    if (err) {
        log_ << "error:" << err.message() << endl;
        return;
    }

    try {
        string file;
        std::istream request_is(&client->buff_);
        request_is >> file >> file;
//        if (file == "/") file = "index.html";
        file = directory_ + file;
        log_ << "req: " << file << endl;
        if (file.find_first_of("?") != string::npos) {
            file.erase(file.find_first_of("?"));
        }
        log_ << "req2: " << file << endl;
        
        log_ << "file: " << file << endl;

        log_ << "her" << endl;
        std::string text;
        boost::filesystem::path path(file);
        if (!exists(path) || is_directory(path)) {
            log_ << "!boost" << endl;
            text = "HTTP/1.0 404 FAIL\r\nContent-Length:0\r\n\r\n";
            write(client->sock_, buffer(text));
        } else {
            log_ << "boost" << endl;
            ifstream result(file);
            text = std::string((std::istreambuf_iterator<char>(result)),
                                std::istreambuf_iterator<char>());
            log_ << "her2" << endl;

            text = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + 
                   std::to_string(text.size()) + 
                   "\r\n\r\n" + text;

            async_write(client->sock_, buffer(text), [](boost::system::error_code, std::size_t){});
        }
        log_ << "text: " << text << endl;
    } catch (exception& e) {
        log_ << "exc: 83: " << e.what() << endl;
        write(client->sock_, buffer("HTTP/1.0 404 FAIL\r\n\r\n"));
    }

    //std::log_ << &client->buff_;
    //async_read_until(client->sock_, client->buff_, "\r\n", std::bind(&HttpServer::readHandler, this, client, _1, _2));
}

void HttpServer::acceptHandler(Client* client, const boost::system::error_code & err) {
    using namespace boost::asio;
    using std::placeholders::_1;
    using std::placeholders::_2;

    async_read_until(client->sock_, client->buff_, "\r\n", std::bind(&HttpServer::readHandler, this, client, _1, _2));
    
    clients_.emplace_back(io_service_);
    acceptor_.async_accept(clients_.back().sock_, std::bind(&HttpServer::acceptHandler, this, &clients_.back(), _1));
}

void HttpServer::start() {
    try {
        for (int i = 0; i < THREADS_COUNT; ++i) {
            threads_[i] = std::thread(std::bind(&HttpServer::run, this));
            log_ << "start worker #" << threads_[i].get_id() << "\n";
        }

        using std::placeholders::_1;
        clients_.emplace_back(io_service_);
        acceptor_.async_accept(clients_.back().sock_, std::bind(&HttpServer::acceptHandler, this, &clients_.back(), _1));

        io_service_.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}


HttpServer::Client::Client(boost::asio::io_service& io_service)
    : sock_(io_service)
{}

//void HttpServer::ready_to_read(
