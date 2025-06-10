#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
    std::cerr << "Slept for 500ms. This line should be red." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
    if(argc >= 1) {
        std::cout << (std::string(argv[0])==""?"Did you add -eargHELLO?":argv[0]) << std::endl;
    } else {
        std::cout << "Did you add -eargHELLO?" << std::endl;
    }
    return 0;
}