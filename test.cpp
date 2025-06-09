#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    std::cerr << "Error1" << std::endl;
    std::cout << "Hello, World!" << std::endl;
    std::cout << "work done";
    std::cout << "lol" << std::endl;
    std::cerr << "whoops" << std::endl;
    if(argc >= 1) {
        std::cout << argv[0];
    }
    return 0;
}