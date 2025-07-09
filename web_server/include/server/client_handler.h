#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <atomic>
#include "data_cache.h"
#include <common/protocol.h>

class ClientHandler {
public:
    ClientHandler(int client_socket, DataCache& cache, std::atomic<bool>& server_running);
    ~ClientHandler();
    
    // Main method to handle client request
    void handleRequest();
    
private:
    int client_socket_;
    DataCache& cache_;
    std::atomic<bool>& server_running_;
    
    // Parse incoming request
    bool parseRequest(const std::string& request, Protocol::Method& method, 
                      std::string& path, std::string& payload);
    
    // Process GET requests
    std::string processGET(const std::string& path);
    
    // Process POST requests
    std::string processPOST(const std::string& path, const std::string& payload);
    
    // Send response to client
    void sendResponse(const std::string& response);
    
    // Get client IP address for logging
    std::string getClientIP() const;
};

#endif // CLIENT_HANDLER_H 