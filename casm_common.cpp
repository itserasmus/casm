#ifndef __CASM_COMMON_INCLUDED_
#define __CASM_COMMON_INCLUDED_

#include <iostream>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>
#include "cout_colors.cpp"
#include "casm_consts.cpp"
#include "casm_msgs.cpp"

using namespace std;

string file_dir(const string &file_path);
string file_name(const string &file_path);
string file_extension(const string &file_name);
string file_without_extension(const string &file_name);
bool is_path_absolute(const string &file_path);
string get_curr_dir();
string get_flags(int n_flags, char** flags);
bool is_flag_reserved(const string flag);
bool file_exists (const string &name);
int run_command(const string cmd);


string file_dir(const string &file_path) {
    size_t last_slash = file_path.find_last_of("/\\");
    if(last_slash != string::npos) {
        return file_path.substr(0, last_slash+1);
    }
    return "";
}


string file_name(const string &file_path) {
    size_t last_slash = file_path.find_last_of("/\\");
    if(last_slash != string::npos) {
        return file_path.substr(last_slash+1);
    }
    return file_path;
}

string file_extension(const string &file_name) {
    size_t last_fill_stop = file_name.find_last_of(".");
    if(last_fill_stop != string::npos) {
        return file_name.substr(last_fill_stop+1);
    }
    return "";
}

string file_without_extension(const string &file_name) {
    size_t last_fill_stop = file_name.find_last_of(".");
    if(last_fill_stop != string::npos) {
        return file_name.substr(0, last_fill_stop);
    }
    return file_name;
}

bool is_path_absolute(const string &file_path) {
    return file_path.length() >= 2 && file_path[1] == ':';
}

string get_curr_dir() {
    wchar_t buffer[MAX_PATH];
    DWORD length = GetCurrentDirectoryW(MAX_PATH, buffer);

    wstring wideString = wstring(buffer, length);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string narrowString = converter.to_bytes(wideString);

    return narrowString;
}

bool is_flag_reserved(const string flag) {
    static const int n_reserved_flags = 6;
    static const string reserved_flags[] = {
        "-no-exc", "-n-e",
        "-to-path-dir", "-t-p-d",
        "-quiet", "-q"
    };

    for(int i = 0; i < n_reserved_flags; i++) {
        if(flag == reserved_flags[i]) {
            return true;
        }
    }
    return false;
}

bool file_exists (const string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

int run_command(const string cmd) {
    return system(cmd.c_str());
}

#endif // __CASM_COMMON_INCLUDED_