# CASM - Compiler And Script Manager
## v_1.1.0

## Overview:
CASM is a lightweight tool for compiling and executing small C/C++ projects, designed to simplify the development
workflow for quick testing and prototyping. Unlike complex build systems like other libraries, CASM aims to provide
straightforward compilation and execution in a single command.

## Features:
- Language Detection: Automatically detects C or C++ based on file extensions, with manual override option.
- Compilation & Execution: Offers optional automatic execution upon successful compilation.
- Profiling: Measures execution time of compiled programs, with varying levels of profiling detail.
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.
- Flexible Configurations: Customize compiler settings, flags, and paths via a `__casm_settings.ini` file.
- Command-Line Simplicity: Clean, intuitive syntax for managing settings and running commands.
- Configurable Output: Supports various naming conventions and directories for output files.
- Error Handling: Provides clear error messages for invalid commands, missing files, or compilation issues.

## Installation:
### Prerequisites:
- GCC or compatible C/C++ compiler.
- [Boost C++ library](https://www.boost.org/) (if not, install it, or go to [src/compile.hpp](src/compile.hpp) and delete all references to boost).
### Building CASM:
- A premake for Windows x686_64 can be found in `./pre_builds/`
1. Clone the repository:
2. Navigate to the cloned directory.
3. Open [CMakeLists](CMakeLists.txt) and scroll to the section marked "IMPORTANT"
4. In the line `set(BOOST_ROOT "C:/path/to/boost")`, set the path to the path to your boost libraries
5. Build with `make` or other compiler.
6. Make sure it works by running `./build/casm.exe -profile test.cpp`. You should get the output
```
C++ file detected. Compiling test.cpp to test.exe...
Profiling test.exe...
-----------------------------
Hello, World!
Error1
Hello, World!
whoops
-----------------------------
Execution successful (68ms)
```
7. If this does not work, make sure that casm.exe exists. If the error persists, refer to the troubleshooting section.

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
- -config        : Confiigure settings and preferences.
- -help          : Display this help message.
- -help config   : Display help message for configuration options.
- -help commands : Display detailed usage of each command.


## Examples:
`casm test.cpp test.exe` - Compiles test.cpp to test.exe
`casm test.cpp` - Compiles test.cpp to test.exe
`casm test.cpp -exec` - Compiles test.cpp to test.exe and executes it
`casm test.cpp -profile` - Compiles test.cpp to test.exe and profiles its execution time
`casm test.cpp -profile-all` - Compiles test.cpp to test.exe, profiles its execution time, and all logs
`casm test.cpp -to-dir-1` - Compiles test.cpp to test.exe and outputs to directory hotlist[1] (hotlist is zero indexed)
`casm -config -help` - Displays detailed help message for configuration options
`casm test.cpp -argO2` - Compiles test.cpp with flag `O2`.
`casm test.cpp -arg-O2` - Compiles test.cpp with flag `-O2`. (notice the difference between `-argO2` and `-arg-O2`)

## Contributing
Contributions are welcome! For now, please submit any issues you find, and we will continue to improve CASM together.

## Troubleshooting:
- Boost library issues: Ensure [Boost](https://www.boost.org/) is correctly installed and the path in `CMakeLists.txt` is set to its root directory.
- Compilation errors: Confirm that your compiler supports C++17 or higher.
This section will be expanded in the future, when common errors are found.

## LICENSE:
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details