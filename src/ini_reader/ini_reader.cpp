#include "ini_reader.hpp"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>


void trim_str(std::string, int&, int&);

INIReader::INIReader(const char* filename) : n_values(0), n_alloc_values(2) {
    this->filename = new char[strlen(filename) + 1];
    strcpy(this->filename, filename);
    values = new ini_value[n_alloc_values];
}
INIReader::~INIReader() {
    delete[] values;
}

ini_value& INIReader::get_value(const char* section, const char* key) {
    for(int i = 0; i < n_values; i++) {
        if(strcmp(values[i].section, section) == 0 && strcmp(values[i].key, key) == 0) {
            return values[i];
        }
    }
    throw std::runtime_error("Key not found");
}
ini_value& INIReader::get_value(const char* key) {
    for(int i = 0; i < n_values; i++) {
        if(strcmp(values[i].key, key) == 0) {
            return values[i];
        }
    }
    throw std::runtime_error("Key not found");
}
ini_value& INIReader::get_value_ensure(const char* section, const char* key, const char* def) {
    for(int i = 0; i < n_values; i++) {
        if(strcmp(values[i].section, section) == 0 && strcmp(values[i].key, key) == 0) {
            return values[i];
        }
    }
    add_value(section, key, def);
    return values[n_values-1];
}

void INIReader::add_value(const char* section, const char* key, const char* val) {
    ini_value value;
    value.section = strdup(section);
    value.key = strdup(key);
    value.val = strdup(val);
    if(n_values >= n_alloc_values) {
        expand_values();
    }
    values[n_values] = value;
    n_values++;
}
void INIReader::remove_value(const char* section, const char* key) {
    int index = -1;
    for(int i = 0; i < n_values; i++) {
        if(strcmp(values[i].section, section) == 0 && strcmp(values[i].key, key) == 0) {
            index = i;
        }
    }
    if(index == -1) {return;}

    values[index].~ini_value();
    n_values--;
    for(int i = index; i < n_values; i++) {
        values[i] = std::move(values[i + 1]);
    }
}

int INIReader::len() {
    return n_values;
}


bool INIReader::read_ini_file() {
    std::fstream file(filename);
    if(!file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return false;
    }
    std::string line;
    std::string curr_section = "General";
    ini_value val;
    int begin, end, equals_pos, name_start, name_end;


    while(std::getline(file, line)) {
        // Check line for 
        // - new section
        // - comments
        // - data type
        // does not support comments at ends of lines

        trim_str(line, begin, end);
        if(end == 0 || line[begin] == '#' || line[begin] == ';' || line[begin] == '\n') {
            continue;
        }
        if(line[begin] == '[') {
            if(line[end] != ']') {
                std::cout << "skipping malformed line" << std::endl;
                continue;
            }
            curr_section = line.substr(begin+1, end-begin-1);
            continue;
        }
        // now, check for normal stuff
        equals_pos = -1;
        for(int i = begin; i < end+1; i++) {
            if(line[i] == '=') {
                equals_pos = i;
                break;
            }
        }
        if(equals_pos == -1) {
            std::cout << "skipping malformed line" << std:: endl;
            continue;
        }
        name_start = equals_pos+1;
        name_end = equals_pos;
        for(int i = begin+1; i < equals_pos; i++) {
            if(line[i] == ' ' || line[i] == '\t') {
                name_end = i;
                break;
            }
        }
        val.key = strdup(line.substr(begin, name_end-begin).c_str());
        val.section = strdup(curr_section.c_str());
        bool in_string = false;
        int last_char = equals_pos+1;
        for(int i = equals_pos+1; i < end; i++) {
            if(line[i] != ' ' && line[i] != '\t') {
                name_start = i;
                break;
            }
        }
        for(int i = equals_pos+1; i < end; i++) {
            switch(line[i]) {
                case ' ':
                case '\t':
                break;
                case '"':
                in_string = !in_string;
                break;
                case ';':
                case '#':
                if(!in_string) {
                    last_char = i-1;
                    goto out_of_loop;
                }
                else {break;}
                default:
                last_char = i+1;
            }
        }
        out_of_loop:
        val.val = strdup(line.substr(name_start, last_char-equals_pos).c_str());
        // add val to values
        if(n_values >= n_alloc_values) {
            expand_values();
        }
        values[n_values] = val;
        n_values++;
        val = ini_value();
    }

    file.close();
    return true;
}

bool INIReader::write_ini_file() {
    std::ofstream file(filename);
    if(!file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return false;
    }
    // map the sections, then write them by sections
    char** sections = new char*[n_values];
    int n_sections = 0;
    for(int i = 0; i < n_values; i++) {
        bool found = false;
        for(int j = 0; j < n_sections; j++) {
            if(strcmp(sections[j], values[i].section) == 0) {
                found = true;
                break;
            }
        }
        if(!found) {
            sections[n_sections++] = strdup(values[i].section);
        }
    }
    for(int i = 0; i < n_sections; i++) {
        file << "[" << sections[i] << "]" << "\n";
        for(int j = 0; j < n_values; j++) {
            if(strcmp(sections[i], values[j].section) == 0) {
                file << values[j].key << "=" << values[j].val << "\n";
            }
        }
        file << "\n";
    }
    file.close();
    return true;
}

inline void INIReader::expand_values() {
    n_alloc_values = 2*(n_alloc_values+1);
    ini_value* new_values = new ini_value[n_alloc_values];
    for(int i = 0; i < n_values; i++) {
        new_values[i].move(values[i]);
    }
    delete[] values;
    values = new_values;
}

void trim_str(std::string line, int &begin, int &end) {
    int len = line.length();
    begin = end = 0;
    for(int i = 0; i < len; i++) {
        if(line[i] != ' ' && line[i] != '\t') {
            begin = i;
            break;
        }
    }
    for(int i = len-1; i > 0; i--) {
        if(line[i] != ' ' && line[i] != '\t') {
            end = i;
            break;
        }
    }
}


namespace ini_parse {
int parse_int(const char* str) {
    int n = 0;
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        n *= 10;
        n += str[i] - '0';
    }
    return n;
}
float parse_float(const char* str) {
    float n = 0;
    int len = strlen(str);
    int decimal_pos = -1;
    for(int i = 0; i < len; i++) {
        if(str[i] == '.') {
            decimal_pos = i;
            break;
        }
    }
    if(decimal_pos == -1) {
        for(int i = 0; i < len; i++) {
            n *= 10;
            n += str[i] - '0';
        }
    } else {
        for(int i = 0; i < decimal_pos; i++) {
            n *= 10;
            n += str[i] - '0';
        }
        float neg_pow = 1;
        for(int i = decimal_pos+1; i < len; i++) {
            neg_pow /= 10.0f;
            n += (str[i] - '0') * neg_pow;
        }
    }
    return n;
}
bool parse_bool(const char* str) {
    return strcmp(str, "true") == 0 || strcmp(str, "True") == 0 || strcmp(str, "TRUE") == 0;
}
int* parse_int_list(const char* str, int& len) {
    bool non_empty = false;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] != ' ' && str[i] != '\t') {
            non_empty = true;
            break;
        }
    }
    if(!non_empty) {
        len = 0;
        return nullptr;
    }
    len = 1;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            len++;
        }
    }
    int* arr = new int[len];
    int curr_num = 0;
    int curr_num_len = 0;
    int curr_num_index = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            arr[curr_num_index] = curr_num;
            curr_num = 0;
            curr_num_len = 0;
            curr_num_index++;
        } else if(str[i] >= '0' && str[i] <= '9') {
            curr_num *= 10;
            curr_num += str[i] - '0';
            curr_num_len++;
        
        }
    }
    arr[curr_num_index] = curr_num;
    return arr;
}
float* parse_float_list(const char* str, int& len) {
    bool non_empty = false;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] != ' ' && str[i] != '\t') {
            non_empty = true;
            break;
        }
    }
    if(!non_empty) {
        len = 0;
        return nullptr;
    }
    len = 1;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            len++;
        }
    }
    float* arr = new float[len];
    float curr_num = 0;
    int curr_num_len = 0;
    int curr_num_index = 0;
    float neg_pow = 1;
    int decimal_pos = -1;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            arr[curr_num_index] = curr_num;
            curr_num = 0;
            curr_num_len = 0;
            curr_num_index++;
            neg_pow = 1;
            decimal_pos = -1;
        } else if(str[i] == '.') {
            decimal_pos = i;
        } else if(!isdigit(str[i])) {
            continue;
        } else if(decimal_pos == -1) {
            curr_num *= 10;
            curr_num += str[i] - '0';
            curr_num_len++;
        } else {
            neg_pow /= 10.0f;
            curr_num += (str[i] - '0') * neg_pow;
        }
    }
    arr[curr_num_index] = curr_num;
    return arr;
}
bool* parse_bool_list(const char* str, int& len) {
    bool non_empty = false;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] != ' ' && str[i] != '\t') {
            non_empty = true;
            break;
        }
    }
    if(!non_empty) {
        len = 0;
        return nullptr;
    }
    len = 1;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            len++;
        }
    }
    bool* arr = new bool[len];
    bool curr_num = false;
    int curr_num_index = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            arr[curr_num_index] = curr_num;
            curr_num = false;
            curr_num_index++;
        } else {
            // check next 4 elements for true and increment i by 3
            if((str[i] == 't' || str[i] == 'T') && str[i+1] == 'r' && str[i+2] == 'u' && str[i+3] == 'e') {
                curr_num = true;
                i += 3;
            }
        }
    }
    arr[curr_num_index] = curr_num;
    return arr;
}
char** parse_string_list(const char* str, int& len) {
    bool non_empty = false;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] != ' ' && str[i] != '\t') {
            non_empty = true;
            break;
        }
    }
    if(!non_empty) {
        len = 0;
        return nullptr;
    }
    bool in_string = false;
    len = 1;
    for(int i = 0; i < strlen(str); i++) {
        switch(str[i]) {
            case '"':
            in_string = !in_string;
            break;
            case ',':
            if(!in_string) {
                len++;
            }
            break;
        }
    }
    char** arr = new char*[len];
    int curr_str_index = 0;
    int curr_str_len = 0;
    int curr_str_start = 0;
    for(int i = 0; i < strlen(str); i++) {
        switch(str[i]) {
            case '"':
            if(!in_string) {
                curr_str_start = i + 1;
            }
            in_string = !in_string;
            break;
            case ',':
            if(!in_string) {
                curr_str_len = i - curr_str_start-1;
                arr[curr_str_index] = new char[curr_str_len + 1];
                strcpy(arr[curr_str_index], str + curr_str_start);
                arr[curr_str_index][curr_str_len] = '\0';
                curr_str_index++;
                curr_str_len = 0;
                curr_str_start = i + 1;
            }
            break;
        }
    }
    curr_str_len = strlen(str) - curr_str_start-1;
    arr[curr_str_index] = new char[curr_str_len + 1];
    strcpy(arr[curr_str_index], str + curr_str_start);
    arr[curr_str_index][curr_str_len] = '\0';
    return arr;
}
}

namespace ini_stringify {
char* stringify_string_list(const char** strs, int len) {
    int accum_len = 1;
    for(int i = 0; i < len; i++) {
        accum_len += strlen(strs[i]);
    }
    char* str = new char[accum_len];
    str[0] = '\0';
    for(int i = 0; i < len; i++) {
        strcat(str, strs[i]);
        if(i != len-1) {
            strcat(str, ",");
        }
    }
    return str;
}
}