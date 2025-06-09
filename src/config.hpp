#pragma once

#include "casm_consts.hpp"
#include "settings.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <windef.h>

using namespace std;

string join_args_from_1(vector<string> args);

int config_settings(vector<string> flags, vector<string> args) {
    if(init_settings()) {
        // Handle single argument cases first, (default to help)

        if(flags.size() >= 2 && (flags[1] == "-view" || flags[1] == "--view")) {
            print_settings();
            return 0;
        }        
        if(args.size() < 1 || args[0] == "help") {
            colout << print_config_help_msg;
            return 0;
        }
        
        if(args[0] == "create-local") {
            colout << CYAN << "Creating local settings file.";
            return create_local_settings_ini() ? 0 : 1;
        }
        if(args[0].rfind("delete-hotlist-", 0) == 0) {
            // delete-hotlist-n and delete-hotlist-all
            if(args[0] == "delete-hotlist-all") {
                colout << CYAN << "Clearing hotlist. This action cannot be undone.\n";
                if(get_y_n("Are you sure you want to clear the hotlist")) {
                    hotlist.clear();
                    colout << CYAN << "Deleted " << n_hotlist << " entries from hotlist.";
                    n_hotlist = 0;
                    return save_settings_and_exit();
                } else {
                    colout << CYAN << "Cancelling delete operation.";
                    return 0;
                }
            } else {
                int index = stoi(args[0].substr(15));
                if(index < 0 || index >= n_hotlist) {
                    colout << RED << "Invalid hotlist index";
                    return 1;
                }
                colout << CYAN << "Deleting hotlist index " << index << ". The hotlist path being deleted is: " << hotlist[index];
                if(get_y_n("Are you sure you want to delete this path?")) {
                    hotlist.erase(hotlist.begin() + index);
                    n_hotlist--;
                    colout << CYAN << "Deleted 1 entry from hotlist.";
                    return save_settings_and_exit();
                } else {
                    colout << CYAN << "Cancelling delete operation.";
                    return 0;
                }
            }
        }
        // Then, ensure 2 arguments before continuing
        if(args.size() < 2) {
            colout << RED << "Not enough arguments provided";
            return 1;
        }
        // handles syntax:
        // casm -config delete-hotlist-n
        // casm -config setting-name value
        // casm -config add-hotlist-path path
        // casm -config reset-settings
        // casm -config create-local
        // possible settings names:
        //    - always-exec
        //    - profile-level
        //    - optimization-level
        //    - c-cpp-compiler
        //    - debug-mode
        //    - input-path
        //    - output-path

        bool modifying_local = std::find(flags.begin(), flags.end(), "-local") != flags.end();
        if(modifying_local && !using_local_settings) {
            colout << RESET << "No local settings file found. To create une, run:\n"
                << YELLOW << "    casm " << BR_BLACK << "-config" << CYAN << " create-local";
            return 1;
        }
        if(args[0] == "always-exec") {
            always_exec = args[1] == "true";
            local_always_exec_exists = modifying_local;
            return save_settings_and_exit();
        } else if(args[0] == "profile-level") {
            profile_level = args[1][0]=='2'?2:args[1][0]=='0'?0:1;
            local_profile_level_exists = modifying_local;
            return save_settings_and_exit();
        } else if(args[0] == "optimization-level") {
            optimization_level = stoi(args[1]);
            local_optimization_level_exists = modifying_local;
            return save_settings_and_exit();
        } else if(args[0] == "c-cpp-compiler") {
            c_cpp_compiler = args[1] == "mingw" || args[1] == "gcc" || args[1] == "g++" ? "gcc" : "clang";
            local_c_cpp_compiler_exists = modifying_local;
            return save_settings_and_exit();
        } else if(args[0] == "debug-mode") {
            debug_mode = args[1] == "true";
            return save_settings_and_exit();
        } else if(args[0] == "input-path") {
            input_path = join_args_from_1(args);
            return save_settings_and_exit();
        } else if(args[0] == "output-path") {
            output_path = join_args_from_1(args);
            return save_settings_and_exit();
        } else if(args[0] == "compiler-flags") {
            compiler_flags = join_args_from_1(args);
            return save_settings_and_exit();
        } else if(args[0] == "runtime-args") {
            runtime_args = join_args_from_1(args);
            return save_settings_and_exit();
        } else if(args[0] == "add-hotlist-path") {
            hotlist.push_back(args[1]);
            n_hotlist++;
            return save_settings_and_exit();
        } else if(args[0] == "reset-settings") {
            colout << CYAN << "Resetting settings to default values. This operation cannot be undone.";
            if(get_y_n("Are you sure you want to reset settings?")) {
                set_defaults();
                return save_settings_and_exit();
            } else {
                colout << "Canceling reset operation.";
                return 0;
            }
        } else {
            colout << RED << "Invalid setting name";
            return 1;
        }
    } else {
        colout << RED << "Could not read settings";
        if(get_y_n("Override with default settings?")) {
            set_defaults();
            return save_settings_and_exit();
        } else {
            return 1;
        }
    }
    return 0;
}

string join_args_from_1(vector<string> args) {
    string s = "";
    for(int i = 1; i < args.size();) {
        if(args[i].find(' ') != string::npos) {s += '"' + args[i] + '"';}
        else {s += args[i];};
        if(++i != args.size()) {s += ' ';}
    }
    return s;
}