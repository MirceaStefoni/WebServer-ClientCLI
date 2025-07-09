# WebServer - Multi-threaded TCP Communication Tool

A complete C++ implementation of a multi-threaded TCP web server with CLI client, featuring modern C++17 design patterns, thread safety, and modular architecture.

## Features

### Server
- **Multi-threaded TCP server** listening on port 8080
- **Thread-safe operations** with proper mutex usage
- **Command support**: `GET /status`, `POST /data`, `GET /shutdown`
- **In-memory caching** of POST data with thread-safe access
- **Comprehensive logging** with timestamps to `log.txt`
- **Graceful shutdown** via `GET /shutdown` command

### Client CLI
- **Command-line interface** for server communication
- **Auto-reconnection** with exponential backoff
- **Error handling** and user-friendly messages
- **Support for GET and POST** requests with payload

### Architecture
- **Modular design** with separate `common/`, `client/`, `server/` modules
- **Protocol abstraction** for easy extensibility
- **Shared logging system** across components
- **Thread-safe utilities** and data structures

## Building the Project

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 6+)
- CMake 3.10 or higher
- Linux/WSL environment (tested on Ubuntu)

### Build Instructions

```bash
# Clone and navigate to project
cd web_server

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .
```

### Build Outputs
- `server` - Multi-threaded TCP server executable
- `client` - CLI client executable

## Usage

### Starting the Server

```bash
# From build directory
./server
```

The server will start listening on port 8080 and log all activities to `log.txt`.

### Using the Client

```bash
# Basic syntax
./client <METHOD> <PATH> [PAYLOAD]

# Examples
./client GET /status
./client POST /data "Hello from client"
./client GET /shutdown
```

### Supported Commands

| Command | Description | Example |
|---------|-------------|---------|
| `GET /status` | Check server status | `./client GET /status` |
| `POST /data <payload>` | Send data to server | `./client POST /data "Hello World"` |
| `GET /shutdown` | Shutdown server | `./client GET /shutdown` |

## Manual Testing

```bash
# Terminal 1: Start server
./server

# Terminal 2: Test commands
./client GET /status
./client POST /data "Test message"
./client GET /shutdown
```

## Project Structure

```
web_server/
├── CMakeLists.txt          # Build configuration
├── README.md              # This file
├── specifications.prd      # Product requirements
├── include/               # Header files
│   ├── client/
│   │   └── tcp_client.h   # TCP client class
│   ├── common/
│   │   ├── logger.h       # Logging utilities
│   │   └── protocol.h     # Protocol definitions
│   └── server/
│       ├── tcp_server.h   # TCP server class
│       ├── client_handler.h # Client request handler
│       └── data_cache.h   # Thread-safe data cache
├── src/                   # Source files
│   ├── client/
│   │   ├── main.cpp       # CLI client main
│   │   └── tcp_client.cpp # TCP client implementation
│   ├── common/
│   │   ├── logger.cpp     # Logging implementation
│   │   └── protocol.cpp   # Protocol utilities
│   └── server/
│       ├── main.cpp       # Server main
│       ├── tcp_server.cpp # TCP server implementation
│       ├── client_handler.cpp # Client handler implementation
│       └── data_cache.cpp # Data cache implementation
├── test/                  # Test directory (empty)
├── build/                 # Build directory
└── log.txt               # Server logs
```

## Features in Detail

### Auto-Reconnection
The client automatically reconnects to the server if connection is lost:
- **Exponential backoff**: 1s, 2s, 3s between attempts
- **Configurable**: Max attempts can be set
- **Transparent**: Automatic retry on send/receive failures

### Thread Safety
- **Mutex protection** for shared data structures
- **Atomic operations** for server control
- **Race condition free** logging and caching

### Error Handling
- **Network errors**: Proper socket error handling
- **Protocol errors**: Invalid command responses
- **Resource management**: RAII and proper cleanup

### Logging
- **Timestamped entries** in `log.txt`
- **Thread-safe logging** across all components
- **Configurable log files** for different components

## Architecture Details

### Server Components
- **TCPServer**: Main server class handling connections
- **ClientHandler**: Processes individual client requests
- **DataCache**: Thread-safe in-memory data storage
- **Logger**: Shared logging functionality
- **Protocol**: Communication protocol definitions

### Client Components
- **TCPClient**: Handles TCP connections and auto-reconnection
- **Main**: Command-line interface and user interaction

## Performance

The server uses a thread-per-connection model and can handle multiple simultaneous connections with proper thread synchronization.

## Troubleshooting

### Common Issues

1. **"Address already in use"**
   - Wait ~60 seconds after server shutdown
   - Use `killall server` to force cleanup

2. **"Connection refused"**
   - Ensure server is running
   - Check firewall settings
   - Verify port 8080 is available

3. **Build errors**
   - Ensure C++17 support
   - Check CMake version (3.10+)
   - Verify all dependencies

### Debug Mode
For detailed logging, check `log.txt` after running commands.

## Future Enhancements

- **Unit tests** for all components
- **Integration tests** for client-server communication
- **TLS/SSL encryption** for secure communication
- **Configuration file** for server settings
- **REST API** with additional HTTP methods
- **Database persistence** for cached data
- **Performance metrics** and monitoring

## Contributing

1. Follow C++17 modern practices
2. Maintain thread safety
3. Add appropriate tests
4. Update documentation
5. Ensure proper error handling

## License

See LICENSE file for details. 