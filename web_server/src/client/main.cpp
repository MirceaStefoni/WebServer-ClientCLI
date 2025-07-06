#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Client starting..." << std::endl;
    if (argc > 1) {
        std::cout << "Command: " << argv[1] << std::endl;
    }
    return 0;
} 