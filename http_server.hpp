#pragma once

#include <string>
#include <list>
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
        boost::asio::streambuf buff_;
    };

private:
    void run();

    void acceptHandler(Client* client, const boost::system::error_code & err);

    void readHandler(Client* client, const boost::system::error_code & err, std::size_t read_bytes);

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

    std::list<Client> clients_;
    std::queue<std::function<void()>> q_;

    std::ofstream log_;
};
