#pragma once

#include <iostream>
#include <boost/process.hpp>
#include "col_cout.hpp"
#include "settings.hpp"
#include "file_stuffs.hpp"

#include <chrono>


void heavy_profile(const char*);

int compile(char** flags, char** args, int n_flags, int n_args) {
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
 
    init_settings();
    bool exec_on_compile = always_exec;
    bool profile_all = profile_level >= 2;
    bool profile = profile_level >= 1;
    
    char* curr_dir = get_curr_dir();

    // path file is being read from (including filename)
    char* source_path = is_path_absolute(args[0]) ? args[0] : add_str(curr_dir, args[0]);
    // path file will be saved to (excluding filename)
    char* dest_dir = nullptr;
    char* dest_name = nullptr;
    char* dest_path = nullptr;

    int n_alloc_compiler_flags_str = 10;
    int len_compiler_flags_str = 0;
    char* compiler_flags_str = new char[n_alloc_compiler_flags_str];
    compiler_flags_str[0] = '\0';

    // -1 for autochecking
    // 0 for c
    // 1 for c++
    int lang = -1;
    
    // check through flags for language, destination, and flags
    for(int i = 0; i < n_flags; i++) {
        if(strcmp(flags[i], "-no-exec") == 0) {exec_on_compile = false;}
        else if(strcmp(flags[i], "-exec") == 0) {exec_on_compile = true;}
        else if(strcmp(flags[i], "-no-profile") == 0) {profile = false; profile_all = false;}
        else if(strcmp(flags[i], "-profile") == 0) {profile = true; profile_all = false;}
        else if(strcmp(flags[i], "-profile-all") == 0) {profile = true; profile_all = true;}
        else if(strcmp(flags[i], "-c") == 0) {lang = 0;}
        else if(strcmp(flags[i], "-cpp") == 0 || strcmp(flags[i], "-c++") == 0) {lang = 1;}
        else if(strncmp(flags[i], "-to-dir-", 8) == 0) {
            int index = strtol(flags[i] + 8, nullptr, 10);
            if(index >= 0 && index <= n_hotlist) {
                dest_dir = hotlist[index];
            }
        } else if(strncmp(flags[i], "-arg", 4) == 0) {
            int len = strlen(flags[i]) - 4;
            if(len_compiler_flags_str + len + 2 >= n_alloc_compiler_flags_str) {
                char* new_compiler_flags_str = new char[(len_compiler_flags_str + len + 2)*2];
                n_alloc_compiler_flags_str = (len_compiler_flags_str + len + 1)*2;
                strcpy(new_compiler_flags_str, compiler_flags_str);
                delete[] compiler_flags_str;
                compiler_flags_str = new_compiler_flags_str;
            }
            compiler_flags_str[len_compiler_flags_str] = ' ';
            strcpy(compiler_flags_str + len_compiler_flags_str + 1, flags[i] + 4);
            len_compiler_flags_str += 1 + len;
        }
    }

    // now determine dest_dir and dest_name
    if(dest_dir == nullptr) { // if it's not been changed to a directory in the hotlist
        dest_dir = curr_dir;  // default save in current directory
    }
    // determine dest_name
    if(n_args < 2) {
        dest_name = file_name_without_extension(args[0]);
        #ifdef _WIN32
        char* new_dest_name = new char[strlen(dest_name) + 4 + 1];
        strcpy(new_dest_name, dest_name);
        strcat(new_dest_name, ".exe");
        delete[] dest_name;
        dest_name = new_dest_name;
        #endif
        colout << RESET << "No output file specified. Outputting to " << CYAN << dest_name << "\n" << RESET;
        dest_path = add_str(dest_dir, dest_name);
    } else if(is_path_absolute(args[1])) { // if the output is an absolute path
        dest_name = file_name(args[1]);
        dest_path = args[1];
    } else { // otherwise, assume it's relative to the current directory
        dest_name = file_name(args[1]);
        dest_path = add_str(dest_dir, dest_name);
    }

    // if the language is undetermined, determine it based on the file extension
    if(lang == -1) {
        if(ends_with(source_path, ".c")) {
            lang = 0;
        } else if(ends_with(source_path, ".cpp") || ends_with(source_path, ".cc") || ends_with(source_path, ".cxx")) {
            lang = 1;
        } else {
            colout << RED << "Error: Language not specified and file extension not recognized\n" << RESET;
            return 1;
        }
    }

    // now, depending on language, build the command
    char* compile_command = nullptr;
    if(lang == 0) {
        colout << B_WHITE << "C" << RESET << " file detected. " << "Compiling " << BR_CYAN << file_name(source_path) << RESET << " to " << BR_CYAN << dest_name << RESET << "..." << "\n";
        compile_command = new char[strlen(dest_path) + strlen(source_path) + strlen(compiler_flags_str) + 10];
        strcpy(compile_command, "gcc ");
        strcat(compile_command, compiler_flags_str);
        strcat(compile_command, " -o ");
        strcat(compile_command, dest_path);
        strcat(compile_command, " ");
        strcat(compile_command, source_path);
    } else if(lang == 1) {
        colout <<  B_WHITE << "C++" << RESET << " file detected. " << "Compiling " << BR_CYAN << file_name(source_path) << RESET << " to " << BR_CYAN << dest_name << RESET << "..." << "\n";
        compile_command = new char[strlen(dest_path) + strlen(source_path) + strlen(compiler_flags_str) + 10];
        strcpy(compile_command, "g++ ");
        strcat(compile_command, compiler_flags_str);
        strcat(compile_command, " -o ");
        strcat(compile_command, dest_path);
        strcat(compile_command, " ");
        strcat(compile_command, source_path);
    }

    // now, compile the file
    int compile_result = system(compile_command);
    if(compile_result != 0) {
        colout << RED << "Error: Compilation failed\n" << RESET;
        delete[] curr_dir;
        delete[] source_path;
        delete[] dest_dir;
        delete[] dest_name;
        delete[] dest_path;
        delete[] compiler_flags_str;
        delete[] compile_command;
        return 1;
    } else if(!exec_on_compile) {
        colout << GREEN << "Compilation successful\n" << RESET;
    }

    if(!exec_on_compile) {
        delete[] curr_dir;
        delete[] source_path;
        delete[] dest_dir;
        delete[] dest_name;
        delete[] dest_path;
        delete[] compiler_flags_str;
        delete[] compile_command;
        return 0;
    }

    // now, run the command based on profile_level.
    // profile_level = 0 => compile and run normally
    // profile_level = 1 => compile and run and profile total execution time
    // profile_level = 2 => compile and run and profile every output line

    if(!profile) {
        // profile_level = 0
        colout << "Running " << BR_CYAN << dest_name << RESET << "...\n" << newline_split;
        int run_result = system(dest_path);
        colout << newline_split;
        if(run_result != 0) {
            colout << RED << "Execution failed with error code: " << run_result << "\n" << RESET;
        } else {
            colout << GREEN << "Execution successful\n" << RESET;
        }
    } else if(profile && !profile_all) {
        // profile_level = 1
        colout << "Profiling " << BR_CYAN << dest_name << RESET << "...\n" << newline_split;
        auto start = std::chrono::high_resolution_clock::now();
        int run_result = system(dest_path);
        colout << newline_split;
        auto end = std::chrono::high_resolution_clock::now();
        if(run_result != 0) {
            colout << RED << "Execution failed with error code: " << run_result << RESET;
        } else {
            colout << GREEN << "Execution successful" << RESET;
        }
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        colout << CYAN << " (" << duration.count() << "ms)\n" << RESET;
    } else if(profile_all) {
        // profile_level = 2
        heavy_profile(dest_path);
    }

    delete_settings();
    return 0;
}

namespace bp = boost::process;

void heavy_profile(const char* file_path) {
    colout << "Heavily Profiling " << BR_CYAN << file_path << RESET << "...\n" << newline_split;

    bp::ipstream pipe_stream; // For capturing standard output
    bp::ipstream error_stream; // For capturing standard error

    std::chrono::_V2::steady_clock::time_point start_time; // forward declare start_time so that threads can see it

    // Thread for capturing standard output
    std::thread output_thread([&]() {
        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line)) {
            auto end_time = std::chrono::steady_clock::now();
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("%5d | %s\n", std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count(), line.c_str());
        }
    });

    // Thread for capturing standard error
    std::thread error_thread([&]() {
        std::string line;
        while (error_stream && std::getline(error_stream, line)) {
            auto end_time = std::chrono::steady_clock::now();
            std::lock_guard<std::mutex> lock(printf_mtx);
            printf("\u001b[31m%5d | %s\n\u001b[0m", std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count(), line.c_str());
        }
    });

    start_time = std::chrono::steady_clock::now();

    // Start the process, redirect stdout and stderr
    bp::child c(file_path, bp::std_out > pipe_stream, bp::std_err > error_stream);

    c.wait(); // Wait for the process to finish
    auto end_time = std::chrono::steady_clock::now();

    // Ensure both threads complete
    output_thread.join();
    error_thread.join();
    
    colout << newline_split;
    if(c.exit_code() != 0) {
        colout << RED << "Execution failed with error code: " << c.exit_code() << RESET;
    } else {
        colout << GREEN << "Execution successful " << RESET;
    }
    colout << CYAN << "(" << std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count() << "ms)\n" << RESET;
}
