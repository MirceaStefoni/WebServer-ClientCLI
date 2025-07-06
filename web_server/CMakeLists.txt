cmake_minimum_required(VERSION 3.10)

# Numele proiectului
project(WebServer CXX)

# Setarea standardului C++ la C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adăugarea directorului 'include' pentru a găsi fișierele header
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Crearea executabilului pentru server
add_executable(server src/server/main.cpp)

# Crearea executabilului pentru client
add_executable(client src/client/main.cpp)

# Mesaj de status pentru utilizator
message(STATUS "CMake configuration complete. You can now build the project.")
message(STATUS "Run 'cmake --build <build_dir>' to compile.") 