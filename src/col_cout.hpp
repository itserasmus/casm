#pragma once

#include <cstdio>
#include <string>
#include <mutex>

#ifndef _WIN32
    #include <cstdlib>
    #include <string>
#endif


/*
comand names -> YELLOW
command params -> CYAN
command flags -> BR_BLACK

file name -> BR_CYAN
error -> RED
success -> BR_GREEN

y/n -> YELLOW

titles -> B_BLUE
subtitles -> BR_CYAN
*/
enum COLOR {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,

    BR_BLACK = 8,
    BR_BLUE = 9,
    BR_GREEN = 10,
    BR_CYAN = 11,
    BR_RED = 12,
    BR_MAGENTA = 13,
    BR_YELLOW = 14,
    BR_WHITE = 15,

    B_BLACK = 16,
    B_BLUE = 17,
    B_GREEN = 18,
    B_CYAN = 19,
    B_RED = 20,
    B_MAGENTA = 21,
    B_YELLOW = 22,
    B_WHITE = 23,

    RESET = 31
};


const char* const bright_cols[] = {
    // Regular
    "\u001b[30m", // BLACK
    "\u001b[34m", // BLUE
    "\u001b[32m", // GREEN
    "\u001b[36m", // CYAN
    "\u001b[31m", // RED
    "\u001b[35m", // MAGENTA
    "\u001b[33m", // YELLOW
    "\u001b[37m", // WHITE

    // Bright
    "\u001b[90m", // BLACK
    "\u001b[94m", // BLUE
    "\u001b[92m", // GREEN
    "\u001b[96m", // CYAN
    "\u001b[91m", // RED
    "\u001b[95m", // MAGENTA
    "\u001b[93m", // YELLOW
    "\u001b[97m", // WHITE

    // Bold
    "\u001b[1;30m", // BLACK
    "\u001b[1;34m", // BLUE
    "\u001b[1;32m", // GREEN
    "\u001b[1;36m", // CYAN
    "\u001b[1;31m", // RED
    "\u001b[1;35m", // MAGENTA
    "\u001b[1;33m", // YELLOW
    "\u001b[1;37m", // WHITE
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "\033[0m"       // RESET
};
const char* const non_bright_cols[] = {
    // Regular
    "\u001b[30m", // BLACK
    "\u001b[34m", // BLUE
    "\u001b[32m", // GREEN
    "\u001b[36m", // CYAN
    "\u001b[31m", // RED
    "\u001b[35m", // MAGENTA
    "\u001b[33m", // YELLOW
    "\u001b[37m", // WHITE

    // Bold
    "\u001b[1;30m", // BLACK
    "\u001b[1;34m", // BLUE
    "\u001b[1;32m", // GREEN
    "\u001b[1;36m", // CYAN
    "\u001b[1;31m", // RED
    "\u001b[1;35m", // MAGENTA
    "\u001b[1;33m", // YELLOW
    "\u001b[1;37m", // WHITE

    // Bold
    "\u001b[1;30m", // BLACK
    "\u001b[1;34m", // BLUE
    "\u001b[1;32m", // GREEN
    "\u001b[1;36m", // CYAN
    "\u001b[1;31m", // RED
    "\u001b[1;35m", // MAGENTA
    "\u001b[1;33m", // YELLOW
    "\u001b[1;37m", // WHITE
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "\033[0m"       // RESET
};


std::mutex printf_mtx;

class ColorOut {
    private:
    bool supports_brightness = true;
    const char* const * cols;
    bool enabled = true;
    public:
    ColorOut() {
        #ifndef _WIN32
        const char* term = std::getenv("TERM");
        supports_brightness = term && (std::string(term).find("xterm") != std::string::npos || std::string(term).find("color") != std::string::npos);
        #endif
        cols = supports_brightness ? bright_cols : non_bright_cols;
    }
    ~ColorOut() {
        std::lock_guard<std::mutex> lock(printf_mtx);
        printf("%s", cols[RESET]);
    }

    ColorOut& operator<<(const char* value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%s", value);
        }
        return *this;
    }
    ColorOut& operator<<(const int value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%d", value);
        }
        return *this;
    }
    ColorOut& operator<<(const float value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%f", value);
        }
        return *this;
    }
    ColorOut& operator<<(const double value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%f", value);
        }
        return *this;
    }
    ColorOut& operator<<(const char value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%c", value);
        }
        return *this;
    }
    ColorOut& operator<<(const bool value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%s", value ? "true" : "false");
        }
        return *this;
    }
    ColorOut& operator<<(const unsigned int value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%u", value);
        }
        return *this;
    }
    ColorOut& operator<<(const long value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%ld", value);
        }
        return *this;
    }
    ColorOut& operator<<(const long long value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%lld", value);
        }
        return *this;
    }
    ColorOut& operator<<(const unsigned long value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%lu", value);
        }
        return *this;
    }
    ColorOut& operator<<(const unsigned long long value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%llu", value);
        }
        return *this;
    }
    

    ColorOut& operator<<(const std::string& value) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%s", value.c_str());
        }
        return *this;
    }


    ColorOut& operator<<(const COLOR color) {
        if(enabled) {
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%s", cols[color]);
        }
        return *this;
    }

    void disable() {
        enabled = false;
    }
    void enable() {
        enabled = true;
    }
};

ColorOut colout;