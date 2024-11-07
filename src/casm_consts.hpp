#pragma once

#include "col_cout.hpp"


constexpr const char* const newline_split = "-----------------------------\n";


#define print_proper_use_msg \
    RED << "Invalid parameter list." << RESET << "\nProper usage of this command is:\n" \
    << print_usage << "\n" << newline_split


#define print_help_msg \
    BR_BLUE << title << "\n" \
    << BR_CYAN << "Overview:\n" << RESET << overview << "\n\n" \
    << BR_CYAN << "Features:\n" << RESET << features << "\n\n" \
    << BR_CYAN << "Usage:\n" << print_usage << "\n\n" \
    << BR_CYAN << "Options:\n" << RESET << options << "\n\n" \
    << "For more details, refer to the documentation or the source code comments.\n" \
    << newline_split

#define print_config_help_msg \
    BR_BLUE << title << "\nCASM Configuration Help" << "\n" \
    << BR_CYAN << "Usage:\n" << RESET << print_config_usage \
    << BR_CYAN << "\n\nActions:\n" << RESET << config_actions \
    << BR_CYAN << "\n\nExamples:\n" << RESET << config_examples << "\n\n" \
    << "For more details, refer to the documentation or the source code comments.\n" \
    << newline_split

#define print_commands_help_msg \
    BR_BLUE << title << "\nCASM Commands Help" << "\n" \
    << BR_CYAN << "Overview:\n" << RESET << "This guide details each CASM command with its syntax and purpose.\n\n" \
    << BR_CYAN << "Commands:\n" << RESET << commands_detailed_list \
    << BR_CYAN << "\n\nExamples:\n" << RESET << commands_examples \
    << "\n\nFor more information, see the documentation or source code comments.\n" \
    << newline_split


constexpr const char* const title = 
"CASM - Compiler And Script Manager\n\
v_1.1.0";

constexpr const char* const overview = "CASM is a lightweight tool for compiling and executing small C/C++ projects, designed to simplify the development workflow for quick testing and prototyping. Unlike complex build systems like other libraries, CASM aims to provide straightforward compilation and execution in a single command.";

const char* features =
"- Language Detection: Automatically detects C or C++ based on file extensions, with manual override option.\n\
- Compilation & Execution: Offers optional automatic execution upon successful compilation.\n\
- Profiling: Measures execution time of compiled programs, with varying levels of profiling detail.\n\
- Hotlist Directory Management: Allows output to specific directories in a configurable hotlist.\n\
- Error Handling: Displays clear error messages for compilation issues and invalid commands.\n\
- Configurable Settings: Configuration options, including preferences and hotlist management.";

constexpr const char* const usage = 
"casm [options] <source_file> [<output_file>]";

#define print_usage \
    BR_YELLOW << "casm" << BR_BLACK << " [options]" << CYAN << " <source_file> [<output_file>]" << RESET

#define print_config_usage \
    BR_YELLOW << "casm" << BR_BLACK << " -config" << CYAN << " action\n" << \
    BR_YELLOW << "casm" << BR_BLACK << " -config" << CYAN << " param <args>" << RESET

constexpr const char* const options = 
"    -cpp, -c++     : Specify language as C++\n\
    -c             : Specify language as C\n\
\n\
    -exec          : Force automatic execution after compilation.\n\
    -no-exec       : Prevent automatic execution after compilation.\n\
    -no-profile    : Preven profiling.\n\
    -profile       : Profiles execution time of the compiled program.\n\
    -profile-all   : Profiles both compilation and execution time.\n\
    -to-dir-n      : Output to directory hotlist[n].\n\
    -arg-cmplr-arg : Pass `complr-arg` to the compiler.\n\
\n\
    -config        : Confiigure settings and preferences.\n\
\n\
    -help          : Display this help message.\n\
    -help config   : Display help message for configuration options.\n\
    -help commands : Display help message for commands.";

constexpr const char* const config_actions =
"    -view               : View current configuration settings.\n\
    help               : Display this help message.\n\
\n\
    delete-hotlist-n    : Delete the nth entry in the hotlist.\n\
    delete-hotlist-all  : Clear all entries from the hotlist.\n\
    reset-settings      : Reset all settings to their default values.\n\
\n\
    always-exec <bool>  : Set 'true' to automatically execute after compiling, 'false' to disable.\n\
    profile-level <n>   : Set profiling detail level:\n\
                          0 - No profiling,\n\
                          1 - Basic profiling,\n\
                          2 - Detailed profiling.\n\
\n\
    hotlist-n <path>    : Set the nth hotlist entry to <path>.\n\
    add-path <path>     : Add <path> to the end of the hotlist.";

constexpr const char* const config_examples =
"    casm -config -view               : Display current configuration.\n\
    casm -config always-exec true     : Enable automatic execution after compilation.\n\
    casm -config delete-hotlist-2     : Delete the third entry in the hotlist.\n\
    casm -config add-path /path/to/dir: Add a new path to the hotlist.\n\
    casm -config profile-level 2      : Set profiling level to detailed.";

constexpr const char* const commands_detailed_list =
"Language Selection:\n\
    -cpp, -c++     : Compile the specified file as C++ code (overrides language detection).\n\
    -c             : Compile the specified file as C code (overrides language detection).\n\
\n\
Execution Control:\n\
    -exec          : Execute the program after successful compilation.\n\
    -no-exec       : Prevent automatic execution of the program after compilation.\n\
\n\
Profiling:\n\
    -no-profile    : Disable profiling.\n\
    -profile       : Profile only the program execution time.\n\
    -profile-all   : Profile both the program exection time and each log.\n\
\n\
Miscellaneous:\n\
    -to-dir-n      : Specify an output directory by using hotlist index n.\n\
    -arg-cmplr-arg : Pass `complr-arg` to the compiler.\n\
\n\
Help Options:\n\
    -help          : Display general help information for all commands.\n\
    -help config   : Display detailed help information for configuration commands.\n\
    -help commands : Display detailed help information for each CASM command.\n\
\n\
Configuration Commands:\n\
    -config            : Enter configuration mode.\n\
    -config -view      : View the current configuration settings.\n\
    -config help       : Display help information for configuration commands.\n\
    -config delete-hotlist-n : Delete the nth entry from the hotlist.\n\
    -config delete-hotlist-all : Clear all entries from the hotlist.\n\
    -config reset-settings : Reset all settings to their default values.\n\
    -config always-exec <bool> : Set automatic execution after compiling.\n\
    -config profile-level <n>  : Set profiling detail level:\n\
                                  0 - No profiling,\n\
                                  1 - Basic profiling,\n\
                                  2 - Detailed profiling.\n\
    -config add-path <path> : Add <path> to the end of the hotlist.\n\
    -config hotlist-n <path> : Set the nth hotlist entry to <path>.\n";

constexpr const char* const commands_examples =
"    casm -cpp source.cpp           : Compile source.cpp as C++ code.\n\
    casm -exec source.c             : Compile and execute source.c if compilation succeeds.\n\
    casm -profile-all source.cpp    : Compile and profile both compilation and execution of source.cpp.\n\
    casm -to-dir-1 source.c         : Compile source.c and output to the first directory in the hotlist.\n\
    casm -config delete-hotlist-2   : Delete the third entry in the hotlist.\n\
    casm -config always-exec true   : Enable automatic execution after compilation.\n\
    casm -config profile-level 2    : Set profiling level to detailed.";


constexpr const char* const settings_path = "casm_settings.ini";