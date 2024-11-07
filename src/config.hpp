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
        if(strncmp(args[0], "delete-hotlist-", 15) == 0) {
            // delete-hotlist-n and delete-hotlist-all
            if(strcmp(args[0], "delete-hotlist-all") == 0) {
                colout << YELLOW << "Clearing hotlist. This action cannot be undone.\n";
                colout << "Are you sure you want to clear the hotlist (y/n): ";
                char c = getchar();
                if(c == 'y' || c == 'Y') {
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
                colout << YELLOW << "Deleting hotlist index " << index << ". The hotlist path being deleted is: " << hotlist[index];
                colout << "Are you sure you want to delete this path? (y/n): ";
                char c = getchar();
                if(c == 'y' || c == 'Y') {
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
        // casm -config hotlist-n path
        // casm -config add-path path
        // casm -config reset-settings
        // possible settings names:
        //    - always-exec
        //    - profile-level
        if(strcmp(args[0], "always-exec") == 0) {
            always_exec = strcmp(args[1], "true") == 0;
            return save_settings_and_exit();
        } else if(strcmp(args[0], "profile-level") == 0) {
            profile_level = args[1][0]=='2'?2:args[1][0]=='0'?0:1;
            return save_settings_and_exit();
        } else if(strncmp(args[0], "hotlist-", 8) == 0) {
            int index = strtol(args[0] + 8, nullptr, 10);
            if(index < 0) {
                colout << RED << "Invalid hotlist index";
                return 1;
            }
            if(index >= n_hotlist) {
                colout << YELLOW << "Hotlist index out of bounds. Add to end? (y/n): ";
                char c = getchar();
                if(c == 'y' || c == 'Y') {
                    char** new_hotlist = new char*[n_hotlist + 1];
                    for(int i = 0; i < n_hotlist; i++) {
                        new_hotlist[i] = hotlist[i];
                    }
                    delete[] hotlist;
                    hotlist = new_hotlist;
                    n_hotlist++;
                    return save_settings_and_exit();
                } else {
                    return 0;
                }
            }
            delete[] hotlist[index];
            hotlist[index] = new char[strlen(args[1]) + 1];
            strcpy(hotlist[index], args[1]);
            return save_settings_and_exit();
        } else if(strcmp(args[0], "add-path") == 0) {
            char** new_hotlist = new char*[n_hotlist + 1];
            for(int i = 0; i < n_hotlist; i++) {
                new_hotlist[i] = hotlist[i];
            }
            delete[] hotlist;
            hotlist = new_hotlist;
            n_hotlist++;
            return save_settings_and_exit();
        } else if(strcmp(args[0], "reset-settings") == 0) {
            colout << YELLOW << "Resetting settings to default values. This operation cannot be undone.";
            colout << "Are you sure you want to reset settings? (y/n): ";
            char c = getchar();
            if(c == 'y' || c == 'Y') {
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
        colout <<  YELLOW << "Override with default settings? (y/n): ";
        char c = getchar();
        if(c == 'y' || c == 'Y') {
            set_defaults();
            return save_settings_and_exit();
        } else {
            return 1;
        }
    }
    return 0;
}