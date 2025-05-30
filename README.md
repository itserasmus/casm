# CASM - Compiler And Script Manager
## v_1.2.0

## Overview:
CASM is a lightweight tool for compiling and executing smallC/C++ projects,
designed to simplify the development workflow for quick testing and prototyping.
Unlike complex build systems like other Make or CMake, CASM aims to provide
straightforward compilation and execution in a single command.

## Features:
- Language Detection: Detects C or C++ based on file extension, with manual override.
- Compilation & Execution: Offers optional automatic execution upon compilation.
- Profiling: Measures execution time of compiled programs, with varying levels of profiling.
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.
- Flexible Configurations: Customize compiler settings and paths via a `casm_settings.ini` file.
- Command-Line Simplicity: Clean, intuitive syntax for managing settings and running commands.
- Configurable Output: Supports various naming conventions and directories for output files.
- Error Handling: Clear error messages for invalid commands, missing files, or compilation issues.

## Installation:

### Prerequisites:
- GCC or compatible C/C++ compiler.

### Building CASM:
- A premake for Windows x86_64 and Windows MSYS2 can be found in `.\pre_builds\`
1. Clone the repository:
2. Navigate to the cloned directory.
3. Run `g++ src\main.cpp -o casm.exe -O3 -pthread` or the equivalent command
    on your compiler.
4. Make sure it works by running `casm.exe test.cpp`. You should get the output
```
No local settings file found. To create one, run:
    casm -config create-local
No output file specified. Outputting to test.exe
C++ file detected. Compiling test.cpp to test.exe...
Compilation successful
Profiling test.exe...
-----------------------------
Hello, World!
Error1
Hello, World!
work donelol
whoops
-----------------------------
Execution successful (33ms)
```
5. If this does not work, make sure that casm.exe exists. If the error persists,
    refer to the troubleshooting section.

## Usage:
casm [-options] [\<source_file>] [\<output_file>]

## Options:
- -cpp, -c++     : Specify language as C++
- -c             : Specify language as C
- -exec          : Force automatic execution after compilation.
- -no-exec       : Prevent automatic execution after compilation.
- -no-profile    : Prevent profiling.
- -profile       : Profiles execution time of the compiled program.
- -profile-all   : Profiles both compilation and execution time.
- -debug         : Build with debugging symbols.
- -release       : Build with optimizations.
- -mingw, -gcc, 
- -g++, -clang   : Specify compiler to use.
- -to-dir-n      : Output to directory hotlist[n].
- -arg-cmplr-arg : Pass `cmplr-arg` to the compiler.
- -config        : Configure settings and preferences.
- -help          : Display this help message.
- -help config   : Display help message for configuration options.
- -help commands : Display detailed usage of each command.


## Examples:
`casm test.cpp test.exe` - Compiles test.cpp to test.exe and executes it
`casm test.cpp` - Compiles test.cpp to test.exe and executes it
`casm test.cpp` - Compiles test.cpp to test.exe and does not execute it
`casm test.cpp -profile` - Compiles test.cpp to test.exe and profiles its execution
    time
`casm test.cpp -profile-all` - Compiles test.cpp to test.exe, profiles its execution
    time, and all logs
`casm test.cpp -to-dir-1` - Compiles test.cpp to test.exe and outputs to directory
    hotlist[1] (hotlist is zero indexed)
`casm -config -help` - Displays detailed help message for configuration options
`casm test.cpp -argO2` - Compiles test.cpp with flag `O2`.
`casm test.cpp -arg-O2` - Compiles test.cpp with flag `-O2`. (note the difference
    between `-argO2` and `-arg-O2`)

## Contributing
Contributions are welcome! For now, please submit any issues you find, and we will
continue to improve CASM together.

## Troubleshooting:
- Compilation errors: Confirm that your compiler supports C++17 or higher. Make sure
    to use the `-pthread` flag.
This section will be expanded in the future, when common errors are found.

## LICENSE:
This project is licensed under the MIT License - see the [LICENSE](LICENSE) for details