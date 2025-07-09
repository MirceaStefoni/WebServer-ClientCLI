cmake_minimum_required(VERSION 3.10)

# Numele proiectului
project(WebServer CXX)

# Setarea standardului C++ la C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Găsește pachetul de threading necesar pentru std::thread
find_package(Threads REQUIRED)

# Adăugarea directorului 'include' pentru a găsi fișierele header
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Definirea surselor comune
set(COMMON_SOURCES
    src/common/protocol.cpp
    src/common/logger.cpp
)

# Definirea surselor pentru server
set(SERVER_SOURCES
    src/server/main.cpp
    src/server/tcp_server.cpp
    src/server/client_handler.cpp
    src/server/data_cache.cpp
    ${COMMON_SOURCES}
)

# Crearea executabilului pentru server
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE Threads::Threads)

# Crearea executabilului pentru client
add_executable(client 
    src/client/main.cpp
    src/client/tcp_client.cpp
    ${COMMON_SOURCES}
)

# Mesaj de status pentru utilizator
message(STATUS "CMake configuration complete. You can now build the project.")
message(STATUS "Run 'cmake --build <build_dir>' to compile.") 