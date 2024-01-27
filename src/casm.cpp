/*
CASM - Compiler and Script Manager
v_1.0.0
*/

#include "casm_common.cpp"
#include "compile_lang.cpp"

int main(int argc, char** argv) {
    int n_inputs = 0;
    int n_flags = 0;
    char** inputs = (char**) malloc(sizeof(char**) * argc);
    char** flags  = (char**) malloc(sizeof(char**) * argc);

    if(argc == 1) {
        col_cout << proper_use_msg << endl;
        return 1;
    }

    char* command = argv[1];
    bool true_command = command[0] == '-';

    for(int i = true_command ? 2 : 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            flags[n_flags++] = argv[i];
        } else {
            inputs[n_inputs++] = argv[i];
        }
    }

    if(!true_command) {
        string source_name(inputs[0], inputs[0] + strlen(inputs[0]));
        if(compile_lang("-" + file_extension(source_name), n_inputs, n_flags, inputs, flags)) {
            return 1;
        }
    } else if(is_lang(string(command))) {
        if(compile_lang(string(command), n_inputs, n_flags, inputs, flags)) {
            return 1;
        }
    }
    
    
    free(inputs);
    free(flags);

    return 0;
}