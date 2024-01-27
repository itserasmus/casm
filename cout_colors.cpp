#ifndef __COUT_COLORS_INCLUDED_
#define __COUT_COLORS_INCLUDED_
#include <iostream>
#include <windows.h>

using namespace std;

/*
comand names -> YELLOW
command params -> CYAN
command flags -> B_BLACK

file name -> B_CYAN
error -> RED
success -> B_GREEN

y/n -> YELLOW
*/
enum COL {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,

    B_BLACK = 8,
    B_BLUE = 9,
    B_GREEN = 10,
    B_CYAN = 11,
    B_RED = 12,
    B_MAGENTA = 13,
    B_YELLOW = 14,
    B_WHITE = 15,

    RESET = 23
};

const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#ifndef _WIN32
const string non_win32[] = {
    "\u001b[30m",
    "\u001b[34m",
    "\u001b[32m",
    "\u001b[36m",
    "\u001b[31m",
    "\u001b[33m",
    "\u001b[35m",
    "\u001b[37m",
    "\u001b[30;m",
    "\u001b[34;m",
    "\u001b[32;m",
    "\u001b[36;m",
    "\u001b[31;m",
    "\u001b[33;m",
    "\u001b[35;m",
    "\u001b[37;m",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "\033[0m"
};
#endif

class ColoredCout {
public:
    template <typename T>
    ColoredCout& operator<<(const T& value) {
        cout << value;
        return *this;
    }

    ColoredCout& operator<<(ostream& (*manip)(ostream&)) {
        cout << manip;
        return *this;
    }

    ColoredCout& operator<<(const COL color) {
        #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, color % 16); // wrap around 16 so that RESET (23) becomes WHITE (7)
        #else
        cout << non_win32[color];
        #endif

        return *this;
    }
};

ColoredCout col_cout;

#endif // __COUT_COLORS_INCLUDED_