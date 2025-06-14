# CASM - Compiler And Script Manager
## v_1.3.1

## Overview:
CASM is a lightweight tool for compiling and executing small C/C++ projects,
designed to simplify the development workflow for quick testing and prototyping.
Unlike more complex build systems such as Make or CMake, CASM aims to provide
straightforward compilation and execution in a single command.

## Features:
- Language Detection: Detects C or C++ based on file extension, with manual override.
- Compilation & Execution: Offers optional automatic execution upon compilation.
- Profiling: Measures execution time of compiled programs, with varying levels of profiling.
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.
- Flexible Configurations: Customize compiler settings and paths via a `casm_settings.ini` file.
- Command-Line Simplicity: Clean, intuiti
ve syntax for managing settings and running commands.
- Configurable Output: Supports various naming conventions and directories for output files.
- Error Handling: Clear error messages for invalid commands, missing files, or compilation issues.

## Installation:

### Prerequisites:
- GCC or compatible C/C++ compiler.

### Building CASM:
- Prebuilt binaries for Linux, Windows x86_64 and MSYS2 can be found in `.\pre_builds\`.
  These have been tested on NixOS, Windows 11, and Windows 11 MSYS2 respectively.
1. Clone the repository: `git clone https://github.com/itserasmus/casm.git`
2. Navigate to the cloned directory. `cd casm`
3. Run `g++ src\main.cpp -o casm -O3 -pthread` or the equivalent command
    on your compiler.
4. Make sure it works by running `./casm test.cpp -eargHELLO -profile-all`. You should get the output
```bash
No local settings file found. To create one, run:
    casm -config create-local
No output file specified. Outputting to test
C++ file detected. Compiling test.cpp to test...
Heavy Profiling /home/NixOS/coding/casm/test...
-----------------------------
    1 | Hello, World!
  501 | Slept for 500ms. This line should be red.
 1001 | HELLO
-----------------------------
Execution successful (1002ms)
```
5. If this does not work, make sure that `./casm` exists. If the error persists,
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
- -arg<cmplr-arg>: Pass `cmplr-arg` to the compiler.
- -earg<exec-arg>: Pass `exec-arg` to the executable.
- -config        : Configure settings and preferences.
- -help          : Display this help message.
- -help config   : Display help message for configuration options.
- -help commands : Display detailed usage of each command.


## Examples:
```bash
casm test.cpp test          # Compiles test.cpp to test and executes it
casm test.cpp               # Compiles test.cpp to test and executes it
casm test.cpp               # Compiles test.cpp to test and does not execute it
casm test.cpp -profile      # Compiles test.cpp to test and profiles its execution time
casm test.cpp -profile-all  # Compiles test.cpp to test, profiles its execution time, and all logs
casm test.cpp -to-dir-1     # Compiles test.cpp to test and outputs to directory hotlist[1]
                            # the hotlist can be configured with casm_settings.ini or `casm -config`
casm -config -help          # Displays detailed help message for configuration options
casm test.cpp -argO2        # Compiles test.cpp with flag `O2`.
casm test.cpp -arg-O2       # Compiles test.cpp with flag `-O2`. (note the difference
                            # between `-argO2` and `-arg-O2`)
casm test.cpp -eargHello    # Runs test.cpp with argument `Hello`.
```

## Contributing
Contributions are welcome! For now, please submit any issues you find, and we will
continue to improve CASM together.

## Troubleshooting:
- Compilation errors: Confirm that your compiler supports C++17 or higher. Make sure
    to use the `-pthread` flag.
This section will be expanded in the future, when common errors are found.

## LICENSE:
This project is licensed under the MIT License - see the [LICENSE](LICENSE) for details