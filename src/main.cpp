/*
CASM - Compiler And Script Manager
v_1.1.0

Overview:
CASM is a lightweight tool for compiling and executing small C/C++ projects, designed to simplify the development
workflow for quick testing and prototyping. Unlike complex build systems like other libraries, CASM aims to provide
straightforward compilation and execution in a single command.

Features:
- Language Detection: Automatically detects C or C++ based on file extensions, with manual override option.
- Compilation & Execution: Offers optional automatic execution upon successful compilation.
- Profiling: Measures execution time of compiled programs, with varying levels of profiling detail.
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.
- Error Handling: Displays clear error messages for compilation issues and invalid commands.
- Configurable Settings: Configuration options, including preferences and hotlist management.

Usage:
    casm [-options] <source_file> [<output_file>]

Options:
-cpp, -c++     : Specify language as C++
-c             : Specify language as C

-exec          : Force automatic execution after compilation.
-no-exec       : Prevent automatic execution after compilation.
-no-profile    : Preven profiling.
-profile       : Profiles execution time of the compiled program.
-profile-all   : Profiles both compilation and execution time.
-to-dir-n      : Output to directory hotlist[n].

-config        : Confiigure settings and preferences.

-help          : Display this help message.
-help config   : Display help message for configuration options.
-help commands : Display detailed usage of each command.


For more details, refer to the documentation or the source code comments.
*/

#include <string.h>

#include "col_cout.hpp"
#include "casm_consts.hpp"
#include "compile.hpp"
#include "config.hpp"

using namespace std;



int main(int argc, char** argv) {
    if(argc < 2) { // no params
        colout << print_proper_use_msg;
        return 1;
    }

    // parse argv
    int n_flags = 0;
    int n_args = 0;
    char** flags = new char*[argc];
    char** args = new char*[argc];

    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            flags[n_flags++] = argv[i];
        } else {
            args[n_args++] = argv[i];
        }
    }
    

    // check for special flags
    if(n_flags > 0) {
        char* f = flags[0];
        if(strcmp(f, "-help") == 0 || strcmp(f, "--help") == 0 || strcmp(f, "-h") == 0 || strcmp(f, "--h") == 0 || strcmp(f, "-?") == 0) {
            if(n_args == 0) {
                colout << print_help_msg;
            } else if(strcmp(args[0], "config") == 0) {
                colout << print_config_help_msg;
            } else if(strcmp(args[0], "commands") == 0) {
                colout << print_commands_help_msg;
            } else {
                colout << print_help_msg;
            }
            delete[] flags;
            delete[] args;
            return 0;
        } else if(strcmp(f, "-config") == 0 || strcmp(f, "--config") == 0) {
            return config_settings(flags, args, n_flags, n_args);
        }
    }

    if(n_args < 1) {
        colout << print_proper_use_msg;
        return 1;

    }
    
    return compile(flags, args, n_flags, n_args);
}