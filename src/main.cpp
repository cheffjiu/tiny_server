#include "server.h"
#include "sql_connection_pool.h"
#include "user_manager.h"
#include "logger.h"

int main() {
    try {
        Logger& logger = Logger::GetInstance(LOGFILE);
        logger.Log(Logger::INFO, "Server initializing...");

        ConnectionPool& pool = ConnectionPool::GetInstance();
        pool.Init("127.0.0.1", 3306, "root", "your_password", "webserver", 4, 10, 3600);

        SqlDatabase db_op(pool);
        UserManager user_manager(db_op);

        Server server("0.0.0.0", 8080, user_manager);
        Router& router = server.GetRouter();
        router.InitRouter(user_manager);

        logger.Log(Logger::INFO, "Server starting on http://0.0.0.0:8080");
        server.Start();
    } catch (const std::exception& e) {
        Logger::GetInstance(LOGFILE).Log(Logger::ERROR, "Server error: " + std::string(e.what()));
        return 1;
    }

    return 0;
}