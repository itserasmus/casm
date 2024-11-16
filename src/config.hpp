#pragma once

#include "settings.hpp"

int config_settings(char** flags, char** args, int n_flags, int n_args) {
    if(init_settings()) {
        // Handle single argument cases first, (default to help)

        if(n_flags >= 2 && (strcmp(flags[1], "-view") == 0 || strcmp(flags[1], "--view") == 0)) {
            print_settings();
            return 0;
        }        
        if(n_args < 1 || strcmp(args[0], "help") == 0) {
            colout << print_config_help_msg;
            return 0;
        }
        
        if(strcmp(args[0], "create-local") == 0) {
            colout << CYAN << "Creating local settings file.";
            return create_local_settings_ini() ? 0 : 1;
        }
        if(strncmp(args[0], "delete-hotlist-", 15) == 0) {
            // delete-hotlist-n and delete-hotlist-all
            if(strcmp(args[0], "delete-hotlist-all") == 0) {
                colout << CYAN << "Clearing hotlist. This action cannot be undone.\n";
                if(get_y_n("Are you sure you want to clear the hotlist")) {
                    for(int i = 0; i < n_hotlist; i++) {
                        delete[] hotlist[i];
                    }
                    delete[] hotlist;
                    hotlist = new char*[1];
                    colout << CYAN << "Deleted " << n_hotlist << " entries from hotlist.";
                    n_hotlist = 0;
                    return save_settings_and_exit();
                } else {
                    colout << CYAN << "Cancelling delete operation.";
                    return 0;
                }
            } else {
                int index = strtol(args[0] + 15, nullptr, 10);
                if(index < 0 || index >= n_hotlist) {
                    colout << RED << "Invalid hotlist index";
                    return 1;
                }
                colout << CYAN << "Deleting hotlist index " << index << ". The hotlist path being deleted is: " << hotlist[index];
                if(get_y_n("Are you sure you want to delete this path?")) {
                    delete[] hotlist[index];
                    for(int i = index; i < n_hotlist - 1; i++) {
                        hotlist[i] = hotlist[i + 1];
                    }
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
        if(n_args < 2) {
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
        if(strcmp(args[0], "always-exec") == 0) {
            always_exec = strcmp(args[1], "true") == 0;
            return save_settings_and_exit();
        } else if(strcmp(args[0], "profile-level") == 0) {
            profile_level = args[1][0]=='2'?2:args[1][0]=='0'?0:1;
            return save_settings_and_exit();
        } else if(strcmp(args[0], "optimization-level") == 0) {
            optimization_level = strtol(args[1], nullptr, 10);
            return save_settings_and_exit();
        } else if(strcmp(args[0], "c-cpp-compiler") == 0) {
            c_cpp_compiler = strcmp(args[1], "mingw") == 0 || strcmp(args[1], "gcc") == 0 || strcmp(args[1], "g++") == 0 ? "gcc" : "clang";
            return save_settings_and_exit();
        } else if(strcmp(args[0], "debug-mode") == 0) {
            debug_mode = strcmp(args[1], "true") == 0;
            return save_settings_and_exit();
        } else if(strcmp(args[0], "input-path") == 0) {
            input_path = args[1];
            return save_settings_and_exit();
        } else if(strcmp(args[0], "output-path") == 0) {
            output_path = args[1];
            return save_settings_and_exit();
        } else if(strcmp(args[0], "add-hotlist-path") == 0) {
            char** new_hotlist = new char*[n_hotlist + 1];
            for(int i = 0; i < n_hotlist; i++) {
                new_hotlist[i] = hotlist[i];
            }
            delete[] hotlist;
            hotlist = new_hotlist;
            n_hotlist++;
            hotlist[n_hotlist - 1] = args[1];
            return save_settings_and_exit();
        } else if(strcmp(args[0], "reset-settings") == 0) {
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