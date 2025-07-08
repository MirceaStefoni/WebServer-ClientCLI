#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

class Logger {
public:
    static void logMessage(const std::string& msg, const std::string& logFile = "log.txt");
    static void logError(const std::string& msg, const std::string& logFile = "log.txt");
    
private:
    static std::mutex log_mutex_;
    static std::string getCurrentTimestamp();
};

#endif // LOGGER_H 