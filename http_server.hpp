#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <fstream>

#include <boost/asio.hpp>

class HttpServer {
public:
    HttpServer(const std::string& host, unsigned short port, const std::string& directory);

    void start();

private:
    struct Client {
        Client(boost::asio::io_service& io_service);

        boost::asio::ip::tcp::socket sock_;
        char buff[1024];
    };

private:
    void run();

private:
    enum { THREADS_COUNT = 3 };

    std::string host_;
    unsigned short port_;
    std::string directory_;

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;

    std::thread threads_[THREADS_COUNT];
    std::mutex m_;
    std::condition_variable cv_;

    std::vector<Client> clients_;
    std::queue<std::function<void()>> q_;

    std::ofstream log_;
};
