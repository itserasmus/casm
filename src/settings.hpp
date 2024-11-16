#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "file_stuffs.hpp"
#include "ini_reader/ini_reader.hpp"


INIReader global_reader(get_global_settings_absolute_path());
INIReader local_reader(get_local_settings_absolute_path());

// global settings casm.exe/../casm_settings.ini
char** hotlist;
int n_hotlist = 0;
bool always_exec;
int profile_level;
int optimization_level;

// per directory settings ./__casm_settings.ini
const char* c_cpp_compiler;
bool debug_mode;
const char* input_path;
const char* output_path;


bool using_local_settings = true;



void delete_settings();



void set_defaults() {
    hotlist = new char*[1];
    n_hotlist = 0;
    always_exec = true;
    profile_level = 1;
    optimization_level = 0;

    c_cpp_compiler = "clang";
    debug_mode = true;
    
    input_path = nullptr;
    output_path = nullptr;
}   

bool create_global_settings_ini_if_not_exist() {
    // check if it exists first
    std::ifstream file(get_global_settings_absolute_path());
    if(file.good()) {
        file.close();
        return true;
    }
    file.close();
    // else create it
    std::ofstream ini_file(get_global_settings_absolute_path());
    if(!ini_file.good()) {
        return false;
    }
    ini_file << "[General]\n";
    ini_file << "hotlist=\n";
    ini_file << "always_exec=false\n";
    ini_file << "profile_level=1\n";
    ini_file << "optimization_level=0\n";
    ini_file.close();
    return true;
}

bool check_if_local_settings_ine_exist() {
    // check if it exists first
    std::ifstream file(get_local_settings_absolute_path());
    if(file.good()) {
        file.close();
        return true;
    }
    file.close();
    colout << RESET << "No local settings file found. To create one, run:\n"
        << YELLOW << "    casm " << BR_BLACK << "-config" << CYAN << " create-local\n";
    using_local_settings = false;
    return false;
}

bool create_local_settings_ini() {
    std::ofstream ini_file(get_local_settings_absolute_path());
    if(!ini_file.good()) {
        ini_file.close();
        return false;
    }
    ini_file << "[General]\n";
    ini_file << "c_cpp_compiler=clang\n";
    ini_file << "debug_mode=true\n";
    ini_file << "input_path=\n";
    ini_file << "output_path=\n";
    ini_file.close();
    return true;
}

bool init_settings() {
    if(!create_global_settings_ini_if_not_exist()) {return false;}
    check_if_local_settings_ine_exist();

    // read global settings
    global_reader.read_ini_file();
    hotlist = ini_parse::parse_string_list(global_reader.get_value_ensure("General", "hotlist", "").val, n_hotlist);
    always_exec = ini_parse::parse_bool(global_reader.get_value_ensure("General", "always_exec", "true").val);
    profile_level = ini_parse::parse_int(global_reader.get_value_ensure("General", "profile_level", "1").val);
    optimization_level = ini_parse::parse_int(global_reader.get_value_ensure("General", "optimization_level", "0").val);

    // read local settings
    if(using_local_settings) {
        local_reader.read_ini_file();
        c_cpp_compiler = ini_parse::parse_string(local_reader.get_value_ensure("General", "c_cpp_compiler", "clang").val);
        debug_mode = ini_parse::parse_bool(local_reader.get_value_ensure("General", "debug_mode", "true").val);
        input_path = ini_parse::parse_string(local_reader.get_value_ensure("General", "input_path", "").val);
        output_path = ini_parse::parse_string(local_reader.get_value_ensure("General", "output_path", "").val);
        if(input_path[0] == '\0') {
            input_path = nullptr;
        }
        if(output_path[0] == '\0') {
            output_path = nullptr;
        }
    }
    
    return true;
}

int save_settings() {
    global_reader.get_value("always_exec").set_val(always_exec ? "true" : "false");
    global_reader.get_value("profile_level").set_val(profile_level==2?"2":profile_level==0?"0":"1");
    global_reader.get_value("hotlist").set_val_no_dup(ini_stringify::stringify_string_list(const_cast<const char**>(hotlist), n_hotlist));
    global_reader.get_value("optimization_level").set_val(optimization_level==2?"2":optimization_level==0?"0":"1");
    if(using_local_settings) {
        local_reader.get_value("c_cpp_compiler").set_val(c_cpp_compiler);
        local_reader.get_value("debug_mode").set_val(debug_mode ? "true" : "false");
        local_reader.get_value("input_path").set_val(input_path);
        local_reader.get_value("output_path").set_val(output_path);
    }
    int wrote_to_local = local_reader.write_ini_file() ? 0 : 1;
    int wrote_to_global = global_reader.write_ini_file() ? 0 : 1;
    return wrote_to_local + wrote_to_global >= 1 ? 1 : 0;
}

int save_settings_and_exit() {
    int ret = save_settings();
    delete_settings();
    return ret;
}

void print_settings() {
    colout << CYAN << "Global Settings:";
    colout << RESET << "\nalways_exec: " << (always_exec ? "true" : "false");
    colout << "\nprofile_level: " << (profile_level==2?"all":profile_level==0?"none":"normal");
    colout << "\nhotlist: ";
    for(int i = 0; i < n_hotlist; i++) {
        colout << "\n  " << hotlist[i];
    }
    if(n_hotlist == 0) {
        colout << "(empty)";
    }
    colout << "\noptimization_level: " << optimization_level;
    if(using_local_settings) {
        colout << CYAN << "\n\nLocal Settings:";
        colout << RESET << "\nc_cpp_compiler: " << c_cpp_compiler;
        colout << "\ndebug_mode: " << (debug_mode ? "true" : "false");
        colout << "\ninput_path: " << (input_path==nullptr?"(unset)":input_path);
        colout << "\noutput_path: " << (output_path==nullptr?"(unset)":output_path);
    } else {
        colout << CYAN << "\nNo local settings found";
    }
}

void delete_settings() {
    for(int i = 0; i < n_hotlist; i++) {
        delete[] hotlist[i];
    }
    delete[] hotlist;
    n_hotlist = 0;
}