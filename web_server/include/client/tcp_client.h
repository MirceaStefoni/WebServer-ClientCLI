#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <common/protocol.h>

class TCPClient {
public:
    TCPClient(const std::string& host = Protocol::DEFAULT_HOST, 
              const std::string& port = Protocol::DEFAULT_PORT,
              bool auto_reconnect = true);
    ~TCPClient();
    
    bool connect();
    void disconnect();
    std::string sendRequest(Protocol::Method method, const std::string& path, const std::string& payload = "");
    
    // Auto-reconnection settings
    void setAutoReconnect(bool enabled) { auto_reconnect_ = enabled; }
    bool getAutoReconnect() const { return auto_reconnect_; }
    void setReconnectAttempts(int attempts) { max_reconnect_attempts_ = attempts; }
    int getReconnectAttempts() const { return max_reconnect_attempts_; }
    
private:
    std::string host_;
    std::string port_;
    int sockfd_;
    bool connected_;
    bool auto_reconnect_;
    int max_reconnect_attempts_;
    
    bool isConnected() const { return connected_; }
    bool tryReconnect();
};

#endif // TCP_CLIENT_H 