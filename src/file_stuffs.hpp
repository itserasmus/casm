#pragma once

#include "col_cout.hpp"

#include <chrono>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

char* get_curr_dir();

char* get_executable_path() {
    #ifdef _WIN32
    char* buffer = new char[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    #else
    char* buffer = new char[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX);
    if(len == -1) {
        return "";
    }
    buffer[len] = '\0';
    #endif
    return buffer;
}

char* get_executable_dir() {
    char* buffer = get_executable_path();
    int len = strlen(buffer);
    for(int i = len-1; i >= 0; i--) {
        if(buffer[i] == '/' || buffer[i] == '\\') {
            buffer[i+1] = '\0';
            break;
        }
    }
    return buffer;
}

char* get_global_settings_absolute_path() {
    char* buffer = get_executable_dir();
    strcat(buffer, "casm_settings.ini");
    return buffer;
}

char* get_local_settings_absolute_path() {
    char* buffer = get_curr_dir();
    strcat(buffer, "__casm_settings.ini");
    return buffer;
}

char* get_curr_dir() {
    #ifdef _WIN32
    char* buffer = new char[MAX_PATH + 2];
    DWORD len = GetCurrentDirectoryA(MAX_PATH, buffer);
    if(len == 0) {
        delete[] buffer;
        return nullptr;
    }
    #else
    char* buffer = new char[PATH_MAX + 2];
    if(getcwd(buffer, PATH_MAX) == nullptr) {
        delete[] buffer;
        return nullptr;
    }
    #endif
    int l = strlen(buffer);
    if(buffer[l-1] != '\\' && buffer[l-1] != '/') {
        buffer[l] = '\\';
        buffer[l+1] = '\0';
    }
    return buffer;
}

bool is_path_absolute(const char* file_path) {
    int len = strlen(file_path);
    return (len >= 2 && file_path[1] == ':') || (len >= 1 && file_path[0] == '/');
}

char* add_str(const char* str1, const char* str2) {
    char* new_str = new char[strlen(str1) + strlen(str2) + 1];
    strcpy(new_str, str1);
    strcpy(new_str + strlen(str1), str2);
    return new_str;
}

char* dup_str(const char* str) {
    char* new_str = new char[strlen(str) + 1];
    strcpy(new_str, str);
    return new_str;
}

char* file_name(const char* file_path) {
    int last_slash = -1;
    for(int i = strlen(file_path)-1; i >= 0; i--) {
        if(file_path[i] == '/' || file_path[i] == '\\') {
            last_slash = i;
            break;
        }
    }
    char* name = new char[strlen(file_path) - last_slash]; // leave space for the null terminator
    strcpy(name, file_path + last_slash + 1);
    return name;
}

char* file_name_without_extension(const char* file_path) {
    int len = strlen(file_path);
    int last_slash = -1;
    int last_dot = len;
    for(int i = len-1; i >= 0; i--) {
        if(file_path[i] == '/' || file_path[i] == '\\') {
            last_slash = i;
            break;
        }
    }
    for(int i = len-1; i >= 0; i--) {
        if(file_path[i] == '.') {
            last_dot = i;
            break;
        }
    }
    char* name = new char[last_dot - last_slash]; // leave space for the null terminator
    strncpy(name, file_path + last_slash + 1, last_dot - last_slash - 1);
    name[last_dot - last_slash - 1] = '\0';
    return name;
}

bool ends_with(const char* str, const char* ending) {
    int len_str = strlen(str);
    int len_ending = strlen(ending);
    if(len_ending > len_str) {
        return false;
    }
    return strncmp(str + len_str - len_ending, ending, len_ending) == 0;
}

