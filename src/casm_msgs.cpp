#include "cout_colors.cpp"

#define proper_use_msg \
    RESET << "Proper usage of this command is:\n" \
        << YELLOW << "casm " << CYAN << "<source.foo> <output.exe>" << B_BLACK << " -flags" << RESET


#define compilation_failed_msg(error) \
    RED << "Compilation failed: " << error << RESET

#define file_type_msg(f_type) \
    B_WHITE << f_type << RESET << "file detected"

#define compiling_msg(source_name, dest_name) \
    "Compiling " << B_CYAN << source_name << RESET << " to " << B_CYAN << dest_name << RESET

