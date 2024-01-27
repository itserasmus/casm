#include "casm_common.cpp"

int compile_lang(string lang, int n_inputs, int n_flags, char** inputs, char** flags) {
    if(n_inputs == 0) {
        col_cout << compilation_failed_msg("Source file not provided") << "\n"
            << proper_use_msg << endl;
        return 1;
    }

    
    string curr_dir = get_curr_dir() + "\\";

    string source_name(inputs[0], inputs[0] + strlen(inputs[0]));
    string source_path = (is_path_absolute(source_name) ? "" : curr_dir) + source_name;
    source_name = file_name(source_path);

    string dest_path;
    string dest_name;


    bool no_exc = false;
    bool to_path_dir = false;
    bool quiet = false;

    string flags_str = "";

    for(int i = 0; i < n_flags; i++) {
        #define reserved_flag(a) \
            if(a) flags_str += " " + flag; \
            else a = true;
        
        string flag = string(flags[i]);

        if(flag == "-no-exc" || flag == "-n-e") {
            reserved_flag(no_exc);
        } else if(flag == "-to-path-dir" || flag == "-t-p-d") {
            reserved_flag(to_path_dir);
        } else if(flag == "-quiet" || flag == "-q") {
            reserved_flag(quiet);
        }
        
        else if(flag.find("-arg-") == 0) {
            flags_str += " " + flag.substr(4);
        } else if(flag.find("-varg-") == 0) {
            flags_str += " " + flag.substr(6);
        } else {
            flags_str += " " + flag;
        }
    }

    if(to_path_dir) {
        dest_name = file_name(inputs[1]);
        dest_path = path_dir + inputs[1];
    } else if(n_inputs == 1) {
        string proposed_output = file_without_extension(source_name) + ".exe";
        col_cout << "Output file not provided\n"
            << "Save output to " << B_CYAN << proposed_output << RESET << "? " << YELLOW << "y/n" << RESET << endl;
        string response;
        cin >> response;

        if(response == "n" || response == "N") {
            col_cout << compilation_failed_msg("Cancelled by user") << endl;
            return 1;
        }

        dest_path = file_dir(source_path) + proposed_output;
        dest_name = proposed_output;
    } else {
        dest_name = inputs[1];
        dest_path = (is_path_absolute(dest_name) ? "" : curr_dir) + dest_name;
        dest_name = file_name(dest_path);
    }

    int compiled;
    string success_msg = "Compilation successful";
    string running_msg = "Running ";
    string failure_msg = "Compiler Error";

    if(lang == "-cpp" || lang == "-c++") {
        col_cout << file_type_msg("C++") << "\n"
            << compiling_msg(source_name, dest_name) << endl;
        
        string cmd = "g++ -o " + dest_path + " " + source_path + flags_str;
        compiled = run_command(cmd);

    } else if(lang == "-c") {
        col_cout << file_type_msg("C") << "\n"
            << compiling_msg(source_name, dest_name) << endl;
        
        string cmd = "gcc -o " + dest_path + " " + source_path + flags_str;
        compiled = run_command(cmd);

    } else {
        col_cout << compilation_failed_msg("Invalid input file") << endl;
        return 1;
    }
    
        
    if(compiled == 0) {
        col_cout << B_GREEN << success_msg << RESET <<endl;
        if(!no_exc) {
            col_cout << running_msg << B_CYAN << dest_name << RESET << "\n--------------------" << endl;
            run_command(dest_path);
        }
    } else {
        col_cout << compilation_failed_msg(failure_msg) << endl;
        return 1;
    }

    return 0;
}

bool is_lang(string command) {
    static const int n_langs = 3;
    static const string langs[] = {"-cpp", "-c++", "-c"};

    for(int i = 0; i < n_langs; i++) {
        if(command == langs[i]) {
            return true;
        }
    }
    return false;
}