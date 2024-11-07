#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "file_stuffs.hpp"
#include "ini_reader/ini_reader.hpp"


INIReader reader(get_settings_absolute_path());
char** hotlist;
int n_hotlist = 0;
bool always_exec;
int profile_level;


void delete_settings();



void set_defaults() {
    hotlist = new char*[1];
    n_hotlist = 0;
    always_exec = true;
    profile_level = 1;
}

bool create_settings_ini_if_not_exist() {
    // check if it exists first
    std::ifstream file(settings_path);
    if(file.good()) {
        file.close();
        return true;
    }
    file.close();
    // else create it
    std::ofstream ini_file(settings_path);
    if(!ini_file.good()) {
        return false;
    }
    ini_file << "[General]\n";
    ini_file << "hotlist=\n";
    ini_file << "always_exec=false\n";
    ini_file << "profile_level=1\n";
    ini_file.close();
    return true;
}

bool init_settings() {
    if(!create_settings_ini_if_not_exist()) {return false;}
    reader.read_ini_file();
    hotlist = ini_parse::parse_string_list(reader.get_value_ensure("General", "hotlist", "").val, n_hotlist);
    always_exec = ini_parse::parse_bool(reader.get_value_ensure("General", "always_exec", "true").val);
    profile_level = ini_parse::parse_int(reader.get_value_ensure("General", "profile_level", "1").val);
    return true;
}

int save_settings() {
    return reader.write_ini_file() ? 0 : 1;
}

int save_settings_and_exit() {
    reader.get_value("always_exec").set_val(always_exec ? "true" : "false");
    reader.get_value("profile_level").set_val(profile_level==2?"2":profile_level==0?"0":"1");
    reader.get_value("hotlist").set_val_no_dup(ini_stringify::stringify_string_list(const_cast<const char**>(hotlist), n_hotlist));
    int ret = reader.write_ini_file() ? 0 : 1;
    delete_settings();
    return ret;
}

void print_settings() {
    reader.get_value("always_exec").set_val(always_exec ? "true" : "false");
    reader.get_value("profile_level").set_val(profile_level==2?"2":profile_level==0?"0":"1");
    reader.get_value("hotlist").set_val_no_dup(ini_stringify::stringify_string_list(const_cast<const char**>(hotlist), n_hotlist));
    colout << "\nhotlist: ";
    for(int i = 0; i < n_hotlist; i++) {
        colout << "\n  " << hotlist[i];
    }
    if(n_hotlist == 0) {
        colout << "(empty)";
    }
}

void delete_settings() {
    for(int i = 0; i < n_hotlist; i++) {
        delete[] hotlist[i];
    }
    delete[] hotlist;
    n_hotlist = 0;
}