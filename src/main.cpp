/*
CASM - Compiler And Script Manager
v_1.3.1

Overview:
CASM is a lightweight tool for compiling and executing small C/C++ projects,
designed to simplify the development workflow for quick testing and prototyping.
Unlike more complex build systems such as Make or CMake, CASM aims to provide
straightforward compilation and execution in a single command.

Features:
- Language Detection: Detects C or C++ based on file extension, with manual override.
- Compilation & Execution: Offers optional automatic execution upon compilation.
- Profiling: Measures execution time of compiled programs, with varying levels of profiling.
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.
- Flexible Configurations: Customize compiler settings and paths via a `casm_settings.ini` file.
- Command-Line Simplicity: Clean, intuitive syntax for managing settings and running commands.
- Configurable Output: Supports various naming conventions and directories for output files.
- Error Handling: Clear error messages for invalid commands, missing files, or compilation issues.

Usage:
    casm [-options] [<source_file>] [<output_file>]

Options:
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
- -argcmplr-arg  : Pass `cmplr-arg` to the compiler.
- -eargexec-arg  : Pass `exec-arg` to the executable.
- -config        : Configure settings and preferences.
- -help          : Display this help message.
- -help config   : Display help message for configuration options.
- -help commands : Display detailed usage of each command.


Configuration:
casm -config                : Confiigure settings and preferences.
    delete-hotlist-n        : Delete nth element of the hotlist.
    add-hotlist-path        : Add a path to the hotlist.
    reset-settings          : Reset all settings to default.
    create-local            : Create a local settings file.
    setting-name <value>    : Set a specific setting.
See `casm -help config` for more details.


Help:
-help          : Display this help message.
-help config   : Display help message for configuration options.
-help commands : Display detailed usage of each command.


For more details, refer to the documentation or the source code comments.
*/

#include <vector>
#include <string>

#include "col_cout.hpp"
#include "casm_consts.hpp"
#include "compile.hpp"
#include "config.hpp"

using namespace std;



int main(int argc, char** argv) {
    // parse argv
    int n_flags = 0;
    int n_args = 0;
    vector<string> flags;
    vector<string> args;

    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            flags.push_back(argv[i]);
            n_flags++;
        } else {
            args.push_back(argv[i]);
            n_args++;
        }
    }
    

    // check for special flags
    if(n_flags > 0) {
        string f = flags[0];
        if(f == "-help" || f == "--help" || f == "-h" || f == "--h" || f == "-?") {
            if(n_args == 0) {
                colout << print_help_msg;
            } else if(args[0] == "config") {
                colout << print_config_help_msg;
            } else if(args[0] == "commands") {
                colout << print_commands_help_msg;
            } else {
                colout << print_help_msg;
            }
            return 0;
        } else if(f == "-config" || f == "--config") {
            return config_settings(flags, args);
        } else if(f == "-version" || f == "--version" || f == "-v" || f == "--v") {
            colout << CYAN << title << RESET;
        }
    }
    
    return compile(flags, args, n_flags, n_args);
}