#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

// Global resources
std::vector<std::string> data_cache;
std::mutex cache_mutex;
std::mutex log_mutex;

void log_message(const std::string& msg) {
    std::lock_guard<std::mutex> guard(log_mutex);
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

    // Write to file
    std::ofstream log_file("log.txt", std::ios::app);
    if (log_file.is_open()) {
        log_file << "[" << ss.str() << "] " << msg << std::endl;
    }
}

void log_error(const char* msg) {
    perror(msg);
    log_message(std::string("ERROR: ") + msg);
}

void handle_client(int client_socket) {
    char recvbuf[DEFAULT_BUFLEN];
    memset(recvbuf, 0, DEFAULT_BUFLEN);

    ssize_t iResult = recv(client_socket, recvbuf, DEFAULT_BUFLEN, 0);
    if (iResult > 0) {
        std::string request(recvbuf);
        log_message("Request received: " + request);

        std::istringstream request_stream(request);
        std::string method, path, payload;
        
        request_stream >> method >> path;
        // Read the rest of the line as payload
        std::getline(request_stream, payload); 
        // Trim leading space from payload
        if (!payload.empty() && payload[0] == ' ') {
            payload = payload.substr(1);
        }

        std::string response;

        if (method == "GET" && path == "/status") {
            response = "200 OK – Server running\n";
        } else if (method == "POST" && path == "/data") {
            std::lock_guard<std::mutex> guard(cache_mutex);
            data_cache.push_back(payload);
            response = "201 Created – Data received\n";
            log_message("Data cached: " + payload);
        } else {
            response = "404 Not Found\n";
        }

        send(client_socket, response.c_str(), response.length(), 0);

    } else if (iResult == 0) {
        log_message("Connection closing...");
    } else {
        log_error("recv");
    }

    close(client_socket);
}

int main() {
    int sockfd, new_sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, DEFAULT_PORT, &hints, &servinfo)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            log_error("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            log_error("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            log_error("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        std::cerr << "server: failed to bind" << std::endl;
        exit(1);
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        log_error("listen");
        exit(1);
    }

    log_message("Server listening on port " + std::string(DEFAULT_PORT) + "...");
    std::cout << "Server listening on port " << DEFAULT_PORT << "..." << std::endl;

    while (true) {
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size = sizeof their_addr;
        int new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_sockfd == -1) {
            log_error("accept");
            continue; // Continue to the next iteration to accept new connections
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(their_addr.ss_family,
            &(((struct sockaddr_in*)&their_addr)->sin_addr),
            s, sizeof s);
        
        std::string conn_msg = "Server: got connection from ";
        conn_msg += s;
        log_message(conn_msg);
        std::cout << conn_msg << std::endl;
        
        // Create a new thread to handle the client
        std::thread(handle_client, new_sockfd).detach();
    }
    
    close(sockfd);

    return 0;
}