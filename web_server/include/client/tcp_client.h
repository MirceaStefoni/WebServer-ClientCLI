#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <common/protocol.h>

class TCPClient {
public:
    TCPClient(const std::string& host = Protocol::DEFAULT_HOST, 
              const std::string& port = Protocol::DEFAULT_PORT);
    ~TCPClient();
    
    bool connect();
    void disconnect();
    std::string sendRequest(Protocol::Method method, const std::string& path, const std::string& payload = "");
    
private:
    std::string host_;
    std::string port_;
    int sockfd_;
    bool connected_;
    
    bool isConnected() const { return connected_; }
};

#endif // TCP_CLIENT_H 