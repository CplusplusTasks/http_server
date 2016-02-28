#pragma once
#include "http_server.hpp"
#include <iostream>

#include <boost/asio.hpp>
#include <boost/aligned_storage.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace ip = boost::asio::ip;

HttpServer::HttpServer(const std::string& host, unsigned short port, const std::string& directory) 
    : host_(host)
    , port_(port)
    , directory_(directory)
    , acceptor_(io_service_, ip::tcp::endpoint(ip::address::from_string(host), port))
    , log_("log")
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

void HttpServer::start() {
    try {
        for (int i = 0; i < THREADS_COUNT; ++i) {
            threads_[i] = std::thread(std::bind(&HttpServer::run, this));
            log_ << "start worker #" << threads_[i].get_id() << "\n";
        }

        while (true) {
            clients_.push_back(Client(io_service_));
            acceptor_.accept(clients_.back().sock_);
            log_ << "accept: " << clients_.back().sock_.remote_endpoint().port() << std::endl;
            //{
                //std::unique_lock<std::mutex> lg(m_);
                //cv_.notify_one();
            //}
        }
        //acceptor_.async_accept(new_session->socket(),
            //boost::bind(&server::handle_accept, this, new_session,
              //boost::asio::placeholders::error));

        //io_service_.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}


HttpServer::Client::Client(boost::asio::io_service& io_service)
    : sock_(io_service)
{}

//void HttpServer::ready_to_read(
