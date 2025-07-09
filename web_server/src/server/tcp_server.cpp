#include <server/tcp_server.h>
#include <server/client_handler.h>
#include <common/logger.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <signal.h>

TCPServer::TCPServer(const std::string& port) 
    : port_(port), sockfd_(-1), running_(false), active_connections_(0) {
    Logger::logMessage("TCPServer created for port " + port_);
}

TCPServer::~TCPServer() {
    stop();
    Logger::logMessage("TCPServer destroyed");
}

bool TCPServer::start() {
    if (running_) {
        Logger::logMessage("Server is already running");
        return true;
    }
    
    if (!initializeSocket()) {
        Logger::logError("Failed to initialize socket");
        return false;
    }
    
    running_ = true;
    Logger::logMessage("Server started on port " + port_);
    std::cout << "Server listening on port " << port_ << "..." << std::endl;
    
    // Start accepting connections
    acceptConnections();
    
    return true;
}

void TCPServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    Logger::logMessage("Server stopping...");
    
    // Close server socket
    if (sockfd_ != -1) {
        close(sockfd_);
        sockfd_ = -1;
    }
    
    // Wait for all client threads to finish
    for (auto& thread : client_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();
    
    Logger::logMessage("Server stopped");
    std::cout << "Server stopped." << std::endl;
}

bool TCPServer::isRunning() const {
    return running_;
}

std::string TCPServer::getPort() const {
    return port_;
}

size_t TCPServer::getActiveConnections() const {
    return active_connections_;
}

bool TCPServer::initializeSocket() {
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((rv = getaddrinfo(NULL, port_.c_str(), &hints, &servinfo)) != 0) {
        Logger::logError("getaddrinfo: " + std::string(gai_strerror(rv)));
        return false;
    }
    
    // Loop through all results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            Logger::logError("socket creation failed");
            continue;
        }
        
        if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            Logger::logError("setsockopt failed");
            close(sockfd_);
            freeaddrinfo(servinfo);
            return false;
        }
        
        if (bind(sockfd_, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_);
            Logger::logError("bind failed");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo);
    
    if (p == NULL) {
        Logger::logError("Failed to bind socket");
        return false;
    }
    
    if (listen(sockfd_, SOMAXCONN) == -1) {
        Logger::logError("listen failed");
        close(sockfd_);
        return false;
    }
    
    return true;
}

void TCPServer::acceptConnections() {
    while (running_) {
        struct sockaddr_storage client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_socket = accept(sockfd_, (struct sockaddr*)&client_addr, &addr_len);
        
        if (client_socket == -1) {
            if (running_) {
                Logger::logError("accept failed");
            }
            continue;
        }
        
        // Get client IP for logging
        char client_ip[INET6_ADDRSTRLEN];
        if (client_addr.ss_family == AF_INET) {
            struct sockaddr_in* addr_in = (struct sockaddr_in*)&client_addr;
            inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET_ADDRSTRLEN);
        } else {
            struct sockaddr_in6* addr_in6 = (struct sockaddr_in6*)&client_addr;
            inet_ntop(AF_INET6, &(addr_in6->sin6_addr), client_ip, INET6_ADDRSTRLEN);
        }
        
        std::string conn_msg = "New connection from " + std::string(client_ip);
        Logger::logMessage(conn_msg);
        std::cout << conn_msg << std::endl;
        
        // Clean up finished threads before creating new ones
        cleanupThreads();
        
        // Create new thread to handle client
        active_connections_++;
        client_threads_.emplace_back(&TCPServer::handleClient, this, client_socket);
    }
}

void TCPServer::handleClient(int client_socket) {
    try {
        ClientHandler handler(client_socket, cache_, running_);
        handler.handleRequest();
    } catch (const std::exception& e) {
        Logger::logError("Exception in client handler: " + std::string(e.what()));
    } catch (...) {
        Logger::logError("Unknown exception in client handler");
    }
    
    active_connections_--;
    Logger::logMessage("Client handler finished, active connections: " + std::to_string(active_connections_));
}

void TCPServer::cleanupThreads() {
    // Remove finished threads
    client_threads_.erase(
        std::remove_if(client_threads_.begin(), client_threads_.end(),
                      [](std::thread& t) {
                          if (t.joinable()) {
                              // Note: This is a simplified cleanup. In a production system,
                              // you might want to use a more sophisticated thread management approach
                              return false;
                          }
                          return true;
                      }),
        client_threads_.end()
    );
}

void TCPServer::setupSignalHandlers() {
    // This could be implemented for graceful shutdown on SIGINT/SIGTERM
    // For now, we'll rely on the shutdown command
} 