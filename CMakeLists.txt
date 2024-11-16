cmake_minimum_required(VERSION 3.10)
project(casm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Source files
set(SOURCES
    src/main.cpp
    src/ini_reader/ini_reader.cpp
)

# Header files
set(HEADERS
    src/compile.hpp
    src/col_cout.hpp
    src/settings.hpp
    src/casm_consts.hpp
    src/config.hpp
    src/file_stuffs.hpp
    src/ini_reader/ini_reader.hpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ---|--- \       / |------\  /-----\  |------\ ---------     /\     \       | --------- |   
#    |    | \   / | |      | |       | |      |     |        /  \    | \     |     |     |   
#    |    |   \   | |------/ |       | |-----/      |       /____\   |   \   |     |     |   
#    |    |       | |        |       | |     \      |      /      \  |     \ |     |
# ___|___ |       | |         \_____/  |      \     |     /        \ |       \     |     O 

# ---------------------------------------
# | THIS IS WHERE THE BOOST ROOT IS SET |
# ---------------------------------------
set(BOOST_ROOT "C:/path/to/boost")

target_include_directories(${PROJECT_NAME} PRIVATE src ${BOOST_ROOT})
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${BOOST_ROOT}/stage/lib/libboost_filesystem-mgw14-mt-x64-1_86.a"
    "${BOOST_ROOT}/stage/lib/libboost_system-mgw14-mt-x64-1_86.a"
    ws2_32
)