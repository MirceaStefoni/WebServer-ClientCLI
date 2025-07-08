#include <client/tcp_client.h>
#include <common/logger.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

TCPClient::TCPClient(const std::string& host, const std::string& port) 
    : host_(host), port_(port), sockfd_(-1), connected_(false) {
}

TCPClient::~TCPClient() {
    disconnect();
}

bool TCPClient::connect() {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(host_.c_str(), port_.c_str(), &hints, &servinfo)) != 0) {
        Logger::logError("getaddrinfo: " + std::string(gai_strerror(rv)));
        return false;
    }
    
    // Loop through results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            Logger::logError("client: socket");
            continue;
        }
        
        if (::connect(sockfd_, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_);
            Logger::logError("client: connect");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo);
    
    if (p == NULL) {
        Logger::logError("client: failed to connect");
        return false;
    }
    
    connected_ = true;
    Logger::logMessage("Connected to server " + host_ + ":" + port_);
    return true;
}

void TCPClient::disconnect() {
    if (connected_ && sockfd_ != -1) {
        close(sockfd_);
        connected_ = false;
        sockfd_ = -1;
        Logger::logMessage("Disconnected from server");
    }
}

std::string TCPClient::sendRequest(Protocol::Method method, const std::string& path, const std::string& payload) {
    if (!connected_) {
        Logger::logError("Not connected to server");
        return "";
    }
    
    std::string request = Protocol::formatRequest(method, path, payload);
    Logger::logMessage("Sending request: " + request);
    
    // Send request
    if (send(sockfd_, request.c_str(), request.length(), 0) == -1) {
        Logger::logError("Failed to send request");
        return "";
    }
    
    // Receive response
    char buffer[Protocol::DEFAULT_BUFLEN];
    memset(buffer, 0, Protocol::DEFAULT_BUFLEN);
    
    ssize_t bytes_received = recv(sockfd_, buffer, Protocol::DEFAULT_BUFLEN - 1, 0);
    if (bytes_received == -1) {
        Logger::logError("Failed to receive response");
        return "";
    } else if (bytes_received == 0) {
        Logger::logMessage("Server closed connection");
        connected_ = false;
        return "";
    }
    
    std::string response(buffer);
    Logger::logMessage("Received response: " + response);
    return response;
} 