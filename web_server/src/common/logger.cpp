#include <common/logger.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

std::mutex Logger::log_mutex_;

void Logger::logMessage(const std::string& msg, const std::string& logFile) {
    std::lock_guard<std::mutex> guard(log_mutex_);
    
    std::ofstream file(logFile, std::ios::app);
    if (file.is_open()) {
        file << "[" << getCurrentTimestamp() << "] " << msg << std::endl;
    }
}

void Logger::logError(const std::string& msg, const std::string& logFile) {
    std::cerr << "ERROR: " << msg << std::endl;
    logMessage("ERROR: " + msg, logFile);
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
} 