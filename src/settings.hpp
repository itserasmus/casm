#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "col_cout.hpp"
#include "file_stuffs.hpp"
#include "ini_reader/ini_reader.hpp"
#include "ini_reader/ini_reader.cpp"

using namespace std;


INIReader global_reader(get_global_settings_absolute_path());
INIReader local_reader(get_local_settings_absolute_path());

// global settings casm.exe/../casm_settings.ini
vector<string> hotlist;
int n_hotlist = 0;
bool always_exec;
int profile_level;
int optimization_level;
string c_cpp_compiler;

// per directory settings ./__casm_settings.ini
bool debug_mode;
string input_path;
string output_path;
string compiler_flags;
string runtime_args;

bool local_always_exec_exists;
bool local_profile_level_exists;
bool local_optimization_level_exists;
bool local_c_cpp_compiler_exists;


bool using_local_settings;



void delete_settings();



void set_defaults() {
    hotlist = {};
    n_hotlist = 0;
    always_exec = true;
    profile_level = 1;
    optimization_level = 0;
    c_cpp_compiler = "mingw";

    debug_mode = true;
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
    ini_file << "always_exec=true\n";
    ini_file << "profile_level=1\n";
    ini_file << "optimization_level=0\n";
    ini_file << "c_cpp_compiler=mingw\n";
    ini_file.close();
    return true;
}

bool check_if_local_settings_ini_exist() {
    // check if it exists first
    std::ifstream file(get_local_settings_absolute_path());
    if(file.good()) {
        file.close();
        using_local_settings = true;
        return true;
    }
    file.close();
    using_local_settings = false;
    return false;
}

// TODO
bool create_local_settings_ini() {
    std::ofstream ini_file(get_local_settings_absolute_path());
    if(!ini_file.good()) {
        ini_file.close();
        return false;
    }
    ini_file << "[General]\n";
    ini_file << "debug_mode=true\n";
    ini_file << "input_path=\n";
    ini_file << "output_path=\n";
    ini_file << "compiler_flags=\n";
    ini_file << "runtime_args=\n";
    ini_file.close();
    return true;
}

bool init_settings() {
    if(!create_global_settings_ini_if_not_exist()) {return false;}
    check_if_local_settings_ini_exist();
    set_defaults();

    // read global settings
    global_reader.read_ini_file();
    char** c_list = ini_parse::parse_string_list(global_reader.get_value_ensure("General", "hotlist", "").val, n_hotlist);
    for(int i = 0; i < n_hotlist; i++) {
        hotlist.push_back(c_list[i]);
        delete[] c_list[i];
    }
    delete[] c_list;

    always_exec = ini_parse::parse_bool(global_reader.get_value_ensure("General", "always_exec", "true").val);
    profile_level = ini_parse::parse_int(global_reader.get_value_ensure("General", "profile_level", "1").val);
    optimization_level = ini_parse::parse_int(global_reader.get_value_ensure("General", "optimization_level", "0").val);
    c_cpp_compiler = ini_parse::parse_string(global_reader.get_value_ensure("General", "c_cpp_compiler", "mingw").val);

    // read local settings
    if(using_local_settings) {
        local_reader.read_ini_file();

        const char* val;

        val = local_reader.get_value("General", "always_exec", local_always_exec_exists).val;
        if(local_always_exec_exists) always_exec = ini_parse::parse_bool(val);

        val = local_reader.get_value("General", "profile_level", local_profile_level_exists).val;
        if(local_profile_level_exists) profile_level = ini_parse::parse_int(val);

        val = local_reader.get_value("General", "optimization_level", local_optimization_level_exists).val;
        if(local_optimization_level_exists) optimization_level = ini_parse::parse_int(val);

        val = local_reader.get_value("General", "c_cpp_compiler", local_c_cpp_compiler_exists).val;
        if(local_c_cpp_compiler_exists) c_cpp_compiler = ini_parse::parse_string(val);

        // only local
        debug_mode = ini_parse::parse_bool(local_reader.get_value_ensure("General", "debug_mode", "true").val);
        input_path = ini_parse::parse_string(local_reader.get_value_ensure("General", "input_path", "").val);
        output_path = ini_parse::parse_string(local_reader.get_value_ensure("General", "output_path", "").val);
        compiler_flags = ini_parse::parse_string(local_reader.get_value_ensure("General", "compiler_flags", "").val);
        runtime_args = ini_parse::parse_string(local_reader.get_value_ensure("General", "runtime_args", "").val);
    }
    
    return true;
}

// TODO
int save_settings() {
    if (!local_always_exec_exists) {global_reader.set_value("General", "always_exec", always_exec ? "true" : "false");}
    if (!local_profile_level_exists) {global_reader.set_value("General", "profile_level", profile_level == 2 ? "2" : profile_level == 0 ? "0" : "1");}
    if (!local_optimization_level_exists) {global_reader.set_value("General", "optimization_level", optimization_level == 2 ? "2" : optimization_level == 0 ? "0" : "1");}
    if (!local_c_cpp_compiler_exists) {global_reader.set_value("General", "c_cpp_compiler", c_cpp_compiler.c_str());}

    char** c_list = new char*[hotlist.size()];
    for (size_t i = 0; i < hotlist.size(); ++i) {
        c_list[i] = new char[hotlist[i].size() + 1];
        strcpy(c_list[i], hotlist[i].c_str());
    }
    std::string result = ini_stringify::stringify_string_list(const_cast<const char**>(c_list), hotlist.size());
    for (size_t i = 0; i < hotlist.size(); ++i) {delete[] c_list[i];}
    delete[] c_list;
    global_reader.set_value_no_dup("General", "hotlist", result.c_str());
    
    int wrote_to_local;
    if(using_local_settings) {
        if(local_always_exec_exists) {local_reader.set_value("General", "always_exec", always_exec ? "true" : "false");}
        if(local_profile_level_exists) {local_reader.set_value("General", "profile_level", profile_level == 2 ? "2" : profile_level == 0 ? "0" : "1");}
        if(local_optimization_level_exists) {local_reader.set_value("General", "optimization_level", optimization_level == 2 ? "2" : optimization_level == 0 ? "0" : "1");}
        if(local_c_cpp_compiler_exists) {local_reader.set_value("General", "c_cpp_compiler", c_cpp_compiler.c_str());}
        
        local_reader.set_value("General", "debug_mode", debug_mode ? "true" : "false");
        local_reader.set_value("General", "input_path", input_path.c_str());
        local_reader.set_value("General", "output_path", output_path.c_str());
        local_reader.set_value("General", "compiler_flags", compiler_flags.c_str());
        local_reader.set_value("General", "runtime_args", runtime_args.c_str());
        wrote_to_local = local_reader.write_ini_file() ? 0 : 1;
    }
    int wrote_to_global = global_reader.write_ini_file() ? 0 : 1;
    return wrote_to_local | wrote_to_global;
}

int save_settings_and_exit() {
    int ret = save_settings();
    delete_settings();
    return ret;
}

// TODO
void print_settings() {
    colout << CYAN << "Global Settings:";
    
    colout << RESET << "\nalways_exec: " << (always_exec ? "true" : "false") << (local_always_exec_exists?" (overridden)":"");
    colout << "\nprofile_level: " << (profile_level==2?"all":profile_level==0?"none":"normal") << (local_profile_level_exists?" (overridden)":"");
    colout << "\noptimization_level: " << optimization_level << (local_optimization_level_exists?" (overridden)":"");
    colout << RESET << "\nc_cpp_compiler: " << c_cpp_compiler << (local_c_cpp_compiler_exists?" (overridden)":"");
    
    colout << "\nhotlist: ";
    for(int i = 0; i < n_hotlist; i++) {
        colout << "\n  " << hotlist[i];
    }
    if(n_hotlist == 0) {
        colout << "(empty)";
    }
    if(using_local_settings) {
        colout << CYAN << "\n\nLocal Settings:";
        colout << "\ndebug_mode: " << (debug_mode ? "true" : "false");
        colout << "\ninput_path: " << (input_path.empty() ? "(unset)" : input_path);
        colout << "\noutput_path: " << (output_path.empty() ? "(unset)" : output_path);
        colout << "\ncompiler_flags: " << (compiler_flags.empty() ? "(unset)" : compiler_flags);
        colout << "\runtime_args: " << (runtime_args.empty() ? "(unset)" : runtime_args);
    } else {
        colout << CYAN << "\nNo local settings found";
    }
}

void delete_settings() {
    hotlist.clear();
    n_hotlist = 0;
}