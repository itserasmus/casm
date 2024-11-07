#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::cerr << "Error1" << std::endl;
    std::cout << "Hello, World!" << std::endl;
    for(volatile int i = 0; i < 1000000000; i++) {
        // "work"
    }
    std::cout << "work done";
    std::cout << "lol" << std::endl;
    std::cerr << "whoops" << std::endl;
    return 0;
}