#ifndef SERVER_H
#define SERVER_H
#include "common.h"
#include "thread_pool.h"
#include "timer.h"
#include "router.h"
#include "user_manager.h"

class Server{
    public:
    Server(const std::string &ip,int port,UserManager& user_manager,size_t thread_count=4);
    ~Server();
    void Start();
    Router& GetRouter();

    private:
        void InitSocket();
        void EventLoop();
        void HandleClient(int fd);

        int server_fd_;
        std::string ip_;
        int port_;
        ThreadPool thread_pool_;
        Timer timer_;
        Router router_;
        Logger& logger_;
};

#endif 