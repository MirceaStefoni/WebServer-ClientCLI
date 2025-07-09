#include <server/client_handler.h>
#include <common/logger.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

ClientHandler::ClientHandler(int client_socket, DataCache& cache, std::atomic<bool>& server_running)
    : client_socket_(client_socket), cache_(cache), server_running_(server_running) {
    Logger::logMessage("ClientHandler created for socket " + std::to_string(client_socket_));
}

ClientHandler::~ClientHandler() {
    if (client_socket_ != -1) {
        close(client_socket_);
        Logger::logMessage("ClientHandler destroyed and socket closed");
    }
}

void ClientHandler::handleRequest() {
    char recvbuf[Protocol::DEFAULT_BUFLEN];
    memset(recvbuf, 0, Protocol::DEFAULT_BUFLEN);
    
    ssize_t bytes_received = recv(client_socket_, recvbuf, Protocol::DEFAULT_BUFLEN - 1, 0);
    if (bytes_received > 0) {
        std::string request(recvbuf);
        Logger::logMessage("Request received from " + getClientIP() + ": " + request);
        
        Protocol::Method method;
        std::string path, payload;
        
        if (parseRequest(request, method, path, payload)) {
            std::string response;
            
            switch (method) {
                case Protocol::Method::GET:
                    response = processGET(path);
                    break;
                case Protocol::Method::POST:
                    response = processPOST(path, payload);
                    break;
                default:
                    response = Protocol::RESPONSE_NOT_FOUND;
                    break;
            }
            
            sendResponse(response);
        } else {
            Logger::logError("Failed to parse request: " + request);
            sendResponse(Protocol::RESPONSE_NOT_FOUND);
        }
    } else if (bytes_received == 0) {
        Logger::logMessage("Client " + getClientIP() + " disconnected");
    } else {
        Logger::logError("recv failed for client " + getClientIP());
    }
}

bool ClientHandler::parseRequest(const std::string& request, Protocol::Method& method, 
                                std::string& path, std::string& payload) {
    std::istringstream request_stream(request);
    std::string method_str;
    
    if (!(request_stream >> method_str >> path)) {
        return false;
    }
    
    method = Protocol::parseMethod(method_str);
    if (method == Protocol::Method::UNKNOWN) {
        return false;
    }
    
    // Read the rest of the line as payload
    std::getline(request_stream, payload);
    // Trim leading space from payload
    if (!payload.empty() && payload[0] == ' ') {
        payload = payload.substr(1);
    }
    
    return true;
}

std::string ClientHandler::processGET(const std::string& path) {
    if (path == Protocol::PATH_STATUS) {
        return Protocol::RESPONSE_STATUS_OK;
    } else if (path == Protocol::PATH_SHUTDOWN) {
        Logger::logMessage("Shutdown request received from " + getClientIP());
        server_running_ = false;
        return "200 OK - Server shutting down";
    } else {
        Logger::logMessage("GET request for unknown path: " + path);
        return Protocol::RESPONSE_NOT_FOUND;
    }
}

std::string ClientHandler::processPOST(const std::string& path, const std::string& payload) {
    if (path == Protocol::PATH_DATA) {
        cache_.addData(payload);
        Logger::logMessage("POST data processed from " + getClientIP() + ": " + payload);
        return Protocol::RESPONSE_DATA_CREATED;
    } else {
        Logger::logMessage("POST request for unknown path: " + path);
        return Protocol::RESPONSE_NOT_FOUND;
    }
}

void ClientHandler::sendResponse(const std::string& response) {
    std::string full_response = response + "\n";
    ssize_t bytes_sent = send(client_socket_, full_response.c_str(), full_response.length(), 0);
    
    if (bytes_sent == -1) {
        Logger::logError("Failed to send response to client " + getClientIP());
    } else {
        Logger::logMessage("Response sent to " + getClientIP() + ": " + response);
    }
}

std::string ClientHandler::getClientIP() const {
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    if (getpeername(client_socket_, (struct sockaddr*)&client_addr, &addr_len) == -1) {
        return "unknown";
    }
    
    char client_ip[INET6_ADDRSTRLEN];
    if (client_addr.ss_family == AF_INET) {
        struct sockaddr_in* addr_in = (struct sockaddr_in*)&client_addr;
        inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET_ADDRSTRLEN);
    } else {
        struct sockaddr_in6* addr_in6 = (struct sockaddr_in6*)&client_addr;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), client_ip, INET6_ADDRSTRLEN);
    }
    
    return std::string(client_ip);
} 