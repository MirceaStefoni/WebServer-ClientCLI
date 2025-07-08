#include <iostream>
#include <string>
#include <vector>
#include <client/tcp_client.h>
#include <common/protocol.h>
#include <common/logger.h>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <METHOD> <PATH> [PAYLOAD]" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " GET /status" << std::endl;
    std::cout << "  " << programName << " POST /data \"Hello from client\"" << std::endl;
    std::cout << std::endl;
    std::cout << "Methods: GET, POST" << std::endl;
    std::cout << "Paths: /status, /data" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== WebServer CLI Client ===" << std::endl;
    
    // Check command line arguments
    if (argc < 3) {
        std::cerr << "Error: Insufficient arguments" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    std::string methodStr = argv[1];
    std::string path = argv[2];
    std::string payload = "";
    
    if (argc >= 4) {
        payload = argv[3];
    }
    
    // Parse method
    Protocol::Method method = Protocol::parseMethod(methodStr);
    if (method == Protocol::Method::UNKNOWN) {
        std::cerr << "Error: Unknown method '" << methodStr << "'" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Validate arguments based on method
    if (method == Protocol::Method::POST && payload.empty()) {
        std::cerr << "Error: POST method requires a payload" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Create TCP client
    TCPClient client;
    
    // Connect to server
    std::cout << "Connecting to server..." << std::endl;
    if (!client.connect()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    
    std::cout << "Connected successfully!" << std::endl;
    
    // Send request
    std::cout << "Sending request: " << Protocol::methodToString(method) << " " << path;
    if (!payload.empty()) {
        std::cout << " " << payload;
    }
    std::cout << std::endl;
    
    std::string response = client.sendRequest(method, path, payload);
    
    if (response.empty()) {
        std::cerr << "Failed to receive response from server" << std::endl;
        return 1;
    }
    
    // Display response
    std::cout << std::endl << "Server response:" << std::endl;
    std::cout << response << std::endl;
    
    // Disconnect
    client.disconnect();
    std::cout << "Disconnected from server." << std::endl;
    
    return 0;
} 