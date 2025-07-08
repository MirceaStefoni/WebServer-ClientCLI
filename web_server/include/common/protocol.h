#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

namespace Protocol {
    // Default server configuration
    const std::string DEFAULT_HOST = "127.0.0.1";
    const std::string DEFAULT_PORT = "8080";
    const int DEFAULT_BUFLEN = 512;
    
    // HTTP-like methods
    enum class Method {
        GET,
        POST,
        UNKNOWN
    };
    
    // Standard responses
    const std::string RESPONSE_STATUS_OK = "200 OK – Server running";
    const std::string RESPONSE_DATA_CREATED = "201 Created – Data received";
    const std::string RESPONSE_NOT_FOUND = "404 Not Found";
    
    // Standard paths
    const std::string PATH_STATUS = "/status";
    const std::string PATH_DATA = "/data";
    const std::string PATH_SHUTDOWN = "/shutdown";
    
    // Utility functions
    Method parseMethod(const std::string& methodStr);
    std::string methodToString(Method method);
    std::string formatRequest(Method method, const std::string& path, const std::string& payload = "");
}

#endif // PROTOCOL_H 