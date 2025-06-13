#pragma once

#include "casm_consts.hpp"
#include "heavy_profile.hpp"
#include "col_cout.hpp"
#include "settings.hpp"
#include "file_stuffs.hpp"

#include <vector>
#include <string>
#include <chrono>

using namespace std;

void heavy_profile(string);

int compile(vector<string> flags, vector<string> args, int n_flags, int n_args) {
    // allows syntax:
    // [] is casm flags
    // casm [] code.ext output.ext
    // casm [] code.ext   ; outputs to file named 'code.ext'
    // casm [] -arg-comp-flag-1 ... code.ext output.ext ; -comp-flag-1 will be passed to compiler (such as -Wall, -v)
    // casm [] -arg-comp-flag-1 ... code.ext

    // casm flags are
    // -cpp, -c++, -c : language specifiers (if none, then guess based on file extension)
    // -to-dir-n : outputs to hotlist[n]
    // -exec, -no-exec : forces/prevents automatic execution after compilation
    // -no-profile : prevents all profiling
    // -profile : profiles only execution time of output
    // -profile-all : profiles output as well
    // -mingw/-gcc/-g++/-clang : compiler to use
    // -debug/-release : build type
    // -arg-comp-flag-1
    // -eargexecution-arg-1

    // If input/output path not specified, it wil read from __casm_settings.ini. If not found,
    // it will infer output from input file name.
 
    init_settings();
    if(n_args < 1 && input_path.empty()) {
        colout << print_proper_use_msg;
        return 1;
    }

    int verbosity = 0; // -1 silent, 1 verbose
    bool exec_on_compile = always_exec;
    bool profile_all = profile_level >= 2;
    bool profile = profile_level >= 1;

    string compiler = c_cpp_compiler;
    bool debug_build = debug_mode;
    
    string curr_dir = get_curr_dir();

    
    // path file is being read from (including filename)
    string source_path = n_args >= 1?
        is_path_absolute(args[0].c_str()) ? args[0] : curr_dir + args[0]
        : is_path_absolute(input_path.c_str()) ? input_path : curr_dir + input_path;
    if(n_args < 1) {
        colout << RESET << "Compiling file " << CYAN << input_path << "\n";
    }
    
    // path file will be saved to (excluding filename)
    string dest_dir;
    string dest_name;
    string dest_path = output_path;
    
    string compiler_flags_str = compiler_flags;
    string execution_args = runtime_args;
    
    int EXIT_CODE = 0;

    // -1 for autochecking
    // 0 for c
    // 1 for c++
    int lang = -1;

    string compile_command;

    int compile_result = 0;
    
    // check through flags for language, destination, and flags
    for(int i = 0; i < n_flags; i++) {
        if(flags[i] == "-no-exec") {exec_on_compile = false;}
        else if(flags[i] == "-exec") {exec_on_compile = true;}
        else if(flags[i] == "-no-profile") {profile = false; profile_all = false;}
        else if(flags[i] == "-profile") {profile = true; profile_all = false;}
        else if(flags[i] == "-profile-all") {profile = true; profile_all = true;}
        else if(flags[i] == "-c") {lang = 0;}
        else if(flags[i] == "-cpp" || flags[i] == "-c++") {lang = 1;}
        else if(flags[i] == "-mingw" || flags[i] == "-gcc" || flags[i] == "-g++") {compiler = "mingw";}
        else if(flags[i] == "-clang") {compiler = const_cast<char*>( "clang");}
        else if(flags[i] == "-debug") {debug_build = true;}
        else if(flags[i] == "-release") {debug_build = false;}
        else if(flags[i] == "-silent") {verbosity = -1;}
        else if(flags[i] == "-verbose") {verbosity = 1;}
        else if(flags[i].rfind("-to-dir-", 0) == 0) {
            int index = stoi(flags[i].substr(8));
            if(index >= 0 && index <= n_hotlist) {
                dest_dir = hotlist[index];
            }
        } else if(flags[i].rfind("-earg", 0) == 0) {
            string arg_val = flags[i].substr(5);
            execution_args += arg_val;
        } else if(flags[i].rfind("-arg", 0) == 0) {
            string arg_val = flags[i].substr(4);
            compiler_flags_str += arg_val;
            if(arg_val == "-O0" || arg_val == "-O1" || arg_val == "-O2" || arg_val == "-O3") {
                optimization_level = -1; // prevent default optimization level from taking place.
            }
        }
    }

    // now determine dest_dir and dest_name
    if(dest_dir.empty()) { // if it's not been changed to a directory in the hotlist
        dest_dir = curr_dir;  // default save in current directory
    }
    // determine dest_name
    if(n_args < 2) {
        if(!dest_path.empty()) {
            if(!is_path_absolute(dest_path.c_str())) {
                dest_path = dest_dir + dest_path;
            }
        }
        dest_name = file_name_without_extension(source_path.c_str());
        #ifdef _WIN32
        dest_name += ".exe";
        #endif
        dest_path = dest_dir + dest_name;
    } else if(is_path_absolute(args[1].c_str())) { // if the output is an absolute path
        dest_name = file_name(args[1].c_str());
        dest_path = args[1];
    } else { // otherwise, assume it's relative to the current directory
        dest_name = file_name(args[1].c_str());
        dest_path = dest_dir + dest_name;
    }


    after_setting_dest_path:
    // if the language is undetermined, determine it based on the file extension
    if(lang == -1) {
        if(ends_with(source_path.c_str(), ".c")) {
            lang = 0; // c
        } else if(ends_with(source_path.c_str(), ".cpp") || ends_with(source_path.c_str(), ".cc") || ends_with(source_path.c_str(), ".cxx")) {
            lang = 1; // cpp
        } else {
            colout << RED << "Error: Language not specified and file extension not recognized\n" << RESET;
            EXIT_CODE = 1;
            return 1;
        }
    }

    // now, depending on language, build the command
    if(lang == 0) {
        if(verbosity >= 0) {
            colout << B_WHITE << "C" << RESET << " file detected. " << "Compiling " << BR_CYAN
                << file_name(source_path.c_str()) << RESET << " to " << BR_CYAN << dest_name << RESET << "..." << "\n";
        }
        compile_command = compiler=="mingw" ? "gcc " : "clang ";
        compile_command += source_path;
        compile_command += " -o ";
        compile_command += dest_path;
        compile_command += " ";
        compile_command += compiler_flags_str;
        if(debug_build) {compile_command += " -g";}
        compile_command += optimization_level==0?"":optimization_level==1?" -O1":optimization_level==2?" -O2":optimization_level==3?" -O3":"";
        if(verbosity == 1) {
            colout << B_WHITE << "Compile Command: " << YELLOW << (compiler=="mingw"?"gcc ":"clang ") << CYAN << source_path
                << BR_BLACK << " -o " << CYAN << dest_path << " " << BR_BLACK << compiler_flags_str;
            if(debug_build) {colout << " -g";}
            colout << (optimization_level==0?"":optimization_level==1?" -O1":optimization_level==2?" -O2":optimization_level==3?" -O3":"")
                << RESET << "\n";
        }
    } else if(lang == 1) {
        if(verbosity >= 0) {
            colout <<  B_WHITE << "C++" << RESET << " file detected. " << "Compiling " << BR_CYAN
                << file_name(source_path.c_str()) << RESET << " to " << BR_CYAN << dest_name << RESET << "..." << "\n";
        }

        compile_command = compiler=="mingw" ? "g++ " : "clang++ ";
        
        compile_command += source_path;
        compile_command += " -o ";
        compile_command += dest_path;
        compile_command += " ";

        compile_command += compiler_flags_str;

        if(debug_build) {compile_command += " -g";}
        compile_command += optimization_level==0?"":optimization_level==1?" -O1":optimization_level==2?" -O2":optimization_level==3?" -O3":"";
        if(verbosity == 1) {
            colout << B_WHITE << "Compile Command: " << YELLOW << (compiler=="mingw"?"g++ ":"clang++ ") << CYAN << source_path
                << BR_BLACK << " -o " << CYAN << dest_path << " " << BR_BLACK << compiler_flags_str;
            if(debug_build) {colout << " -g";}
            colout << (optimization_level==0?"":optimization_level==1?" -O1":optimization_level==2?" -O2":optimization_level==3?" -O3":"")
                << RESET << "\n";
        }
    }
    // now, compile the file

    compile_result = system(compile_command.c_str());
    if(compile_result != 0) {
        colout << RED << "Error: Compilation failed\n" << RESET;
        EXIT_CODE = 1;
        return 1;
    } else {
        colout << GREEN << "Compilation successful\n" << RESET;
    }

    // now, run the command based on profile_level.
    // profile_level = 0 => compile and run normally
    // profile_level = 1 => compile and run and profile total execution time
    // profile_level = 2 => compile and run and profile every output line

    string run_command = dest_path;
    if(!execution_args.empty()) {
        run_command += " " + execution_args;
    }

    if(profile_all) {
        heavy_profile(dest_path, execution_args);
        delete_settings();
        return EXIT_CODE;
    }

    if(!profile) {
        // profile_level = 0
        colout << "Running " << BR_CYAN << dest_name << RESET << "...\n" << newline_split;
    } else {
        colout << "Profling " << BR_CYAN << dest_name << RESET << "...\n" << newline_split;
    }

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    vector<char*> argv;
    stringstream ss(execution_args);
    string word;
    char* wordC;
    while (ss >> word) {
        wordC = new char[word.size()+1];
        strncpy(wordC, word.c_str(), word.size());
        wordC[word.size()] = '\0';
        argv.push_back(wordC);
    }
    argv.push_back(nullptr);

    auto start = std::chrono::steady_clock::now();
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        execvp(dest_path.c_str(), argv.data());
        perror("execvp");
        exit(errno);
    }
    auto end = std::chrono::steady_clock::now();

    for(int i = 0; argv[i] != nullptr; i++) {delete[] argv[i];}

    int status;
    waitpid(pid, &status, 0);
    int run_result;
    if (WIFEXITED(status)) {
        run_result = WEXITSTATUS(status);
    } else {
        run_result = 128 + WTERMSIG(status);
    }
#else
std::string full_cmd = dest_path + " " + execution_args;

auto start = std::chrono::steady_clock::now();
int run_result = system(full_cmd.c_str());
auto end = std::chrono::steady_clock::now();

#endif
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    for(int i = 0; argv[i] != nullptr; i++) {delete[] argv[i];}
#endif

    colout << newline_split;
    if(run_result != 0) {
        colout << RED << "Execution failed with error code: " << run_result << "\n" << RESET;
    } else {
        colout << GREEN << "Execution successful\n" << RESET;
    }
    
    if(profile) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        colout << CYAN << " (" << duration.count() << "ms)\n" << RESET;
    }

    delete_settings();

    return EXIT_CODE;
}
