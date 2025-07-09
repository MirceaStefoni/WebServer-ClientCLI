#include <iostream>
#include <server/tcp_server.h>
#include <common/logger.h>
#include <common/protocol.h>
#include <signal.h>

int main() {
    std::cout << "=== WebServer - Multi-threaded TCP Server ===" << std::endl;
    
    Logger::logMessage("=== Server Starting ===");
    
    // Create server instance
    TCPServer server(Protocol::DEFAULT_PORT);
    
    // Setup signal handler for graceful shutdown
    signal(SIGINT, [](int sig) {
        Logger::logMessage("Received SIGINT, shutting down gracefully...");
        std::cout << "\nReceived SIGINT, shutting down gracefully..." << std::endl;
        exit(0);
    });
    
    // Start the server
    if (!server.start()) {
        Logger::logError("Failed to start server");
        std::cerr << "Failed to start server. Check logs for details." << std::endl;
        return 1;
    }
    
    Logger::logMessage("=== Server Shutdown Complete ===");
    std::cout << "Server shutdown complete." << std::endl;
    
    return 0;
}