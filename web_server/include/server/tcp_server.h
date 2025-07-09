#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "data_cache.h"
#include <common/protocol.h>

class TCPServer {
public:
    TCPServer(const std::string& port = Protocol::DEFAULT_PORT);
    ~TCPServer();
    
    // Start the server
    bool start();
    
    // Stop the server
    void stop();
    
    // Check if server is running
    bool isRunning() const;
    
    // Get server port
    std::string getPort() const;
    
    // Get number of active connections
    size_t getActiveConnections() const;
    
private:
    std::string port_;
    int sockfd_;
    std::atomic<bool> running_;
    std::atomic<size_t> active_connections_;
    
    DataCache cache_;
    std::vector<std::thread> client_threads_;
    
    // Initialize socket and bind to port
    bool initializeSocket();
    
    // Accept incoming connections
    void acceptConnections();
    
    // Handle individual client in separate thread
    void handleClient(int client_socket);
    
    // Cleanup finished threads
    void cleanupThreads();
    
    // Setup signal handlers for graceful shutdown
    void setupSignalHandlers();
};

#endif // TCP_SERVER_H 