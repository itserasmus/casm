#pragma once

#include <cstring>

/**
 * @class ini_value
 * @brief This class represents a value in an INI file.
 * It holds the key, section, and value.
 * 
 * @details The ini_value class is used to store the values in the INI file.
 * The functions in the `ini_parse` namespace are used to parse the value.
 * It stores the key, section, and value as `char*`s. It is used by `INIReader`.
 * 
 * @note This class stores the value as a `char*`, and it must be
 * parsed using the functions in the `ini_parse` namespace
 */
class ini_value {
    public:
    /// @brief Holds the key (name) of the value
    const char* key;
    /// @brief Holds the section (defaults to "General") that the value is in
    const char* section;
    /** 
     * @brief Holds the content of the ini_value
     * @note The value is stored as a `char*`, and must be parsed using the
     * functions in the `ini_parse` namespace
     * @note The value is a raw c-string, requiring manual parsing and management
     */
    const char* val;

    /// @brief Default constructor
    ini_value() : key(nullptr), section(nullptr), val(nullptr) {}
    
    /**
     * @brief Copy constructor
     * @param other The other ini_value to copy
     */
    ini_value(const ini_value &other) {
        key = strdup(other.key);
        section = strdup(other.section);
        val = strdup(other.val);
    }

    /**
     * @brief Copy assignment operator (duplicative copy)
     * @param other The other ini_value to copy
     */
    ini_value& operator=(const ini_value &other) {
        if(this == &other) {
            return *this;
        }
        this->~ini_value();
        
        key = strdup(other.key);
        section = strdup(other.section);
        val = strdup(other.val);
        return *this;
    }

    /**
     * @brief Move method (destructive copy)
     * @param other The other ini_value to move
     * @note The other ini_value will be invalidated by this operation
     */
    void move(ini_value &other) {
        if(this == &other) {return;}
        this->~ini_value();

        key = other.key;
        section = other.section;
        val = other.val;

        other.key = nullptr;
        other.section = nullptr;
        other.val = nullptr;
    }

    /**
     * @brief Set the value of the ini_value
     * @param val The value to set
     * @note This value is duplicated, thus, owned by this object
     */
    void set_val(const char* val) {
        delete[] this->val;
        this->val = strdup(val);
    }

    /**
     * @brief Set the value of the ini_value without duplicating it
     * @param val The value to set
     * @note This method transfers ownership of the value to this object
     * @note the value is not duplicated, but ownership is transferred
     */
    void set_val_no_dup(const char* val) {
        delete[] this->val;
        this->val = val;
    }

    /// @brief Destructor
    ~ini_value() {
        delete[] key;
        delete[] section;
        delete[] val;
    }
};

/**
 * @class INIReader
 * @brief This class represents an INI file.
 * It holds the filename, and a list of ini_value objects.
 * 
 * @details The INIReader class is used to read from and write to the INI file.
 * The filepath is given in the constructor, and the `read_ini_file()` function
 * is called to read the file. The `add_value()` and `remove_value()` functions
 * are used to modify the INI file. The `write_ini_file()` function is used
 * to save the changes to the INI file.
 * All values are stored in the `ini_value` list, and all changes are made only
 * to this list. The `ini_value` list is not saved to the INI file until the 
 * `write_ini_file()` function is called.
 */
class INIReader {
    public:
    /** @brief Holds the filename of the INI file
     * @attention The `filename` given will be treated as relative to the current
     * working directory. (often the current directory in a terminal). If
     * the program may be called from a different directory, the program
     * should give an absolute path to the INI file.
     */
    char* filename;
    
    /// @brief Default constructor
    /// @param filename The filename of the INI file
    INIReader(const char* filename);
    /// @brief Destructor
    ~INIReader();
    
    /**
     * @brief Reads the INI file, and stores the values in the ini_value objects
     * @return `true` if the file was read successfully, `false` otherwise
     * @note The ini_value objects are stored in the `values` array
     */
    bool read_ini_file();
    /**
     * @brief Writes the INI file, and stores the values in the ini_value objects.
     * If the file does not exist, it will be created. If the file has any data,
     * it will be overwritten.
     * @return `true` if the file was written successfully, `false` otherwise
     */
    bool write_ini_file();
    
    /**
     * @brief Gets the value of a key by section and key
     * @param section The section of the key
     * @param key The key of the value
     * @return The value of the key, or an empty ini_value if the key does not exist
     */
    ini_value& get_value(const char* section, const char* key);
    /**
     * @brief Gets the value of a key by key
     * @param key The key of the value
     * @return The value of the key, or an empty ini_value if the key does not exist
     * @note The *first* matching key will be returned
     */
    ini_value& get_value(const char* key);
    /**
     * @brief Gets the value of a key by section and key, and creates it if it does not exist
     * @param section The section of the key
     * @param key The key of the value
     * @param def The default value of the key
     * @return The value of the key, or the default value if the key does not exist
     */
    ini_value& get_value_ensure(const char* section, const char* key, const char* def);

    /**
     * @brief Adds a value to the INI file
     * @param section The section of the key
     * @param key The key of the value
     * @param val The value of the key
     * @note To save changes, `write_ini_file()` must be called
     */
    void add_value(const char* section, const char* key, const char* val);
    /**
     * @brief Removes a value from the `ini_value` list
     * @param section The section of the key
     * @param key The key of the value
     * @note To save changes, `write_ini_file()` must be called
     */
    void remove_value(const char* section, const char* key);

    /// @brief Returns the number of values in the `ini_value` list
    /// @return The number of values in the `ini_value` list
    int len();

    private:
    /// @brief The internal list of ini_value objects
    ini_value* values;
    /// @brief The amount of memory allocated for the `ini_value` list
    int n_alloc_values;
    /// @brief The number of values in the `ini_value` list
    int n_values;

    // @brief Doubles the amount of memory allocated for the `ini_value` list
    void expand_values();
};


/**
 * @namespace ini_parse
 * @brief Namespace for functions that parse `char*` into other types
 */
namespace ini_parse {
    /// @brief Parses a string into a string
    /// @param str The string to parse
    /// @return The parsed string
    char* parse_string(const char* str);
    /// @brief Parsess a string into an integer
    /// @param str The string to parse
    /// @return The parsed integer
    int parse_int(const char* str);

    /// @brief Parses a string into a float
    /// @param str The string to parse
    /// @return The parsed float
    float parse_float(const char* str);

    /// @brief Parses a string into a boolean
    /// @param str The string to parse
    /// @return The parsed boolean
    bool parse_bool(const char* str);

    /// @brief Parses a string into an array of integers
    /// @param str The string to parse
    /// @param len Will be set to the length of the array
    /// @return The parsed array of integers
    int* parse_int_list(const char* str, int& len);

    /// @brief Parses a string into an array of floats
    /// @param str The string to parse
    /// @param len Will be set to the length of the array
    /// @return The parsed array of floats
    float* parse_float_list(const char* str, int& len);

    /// @brief Parses a string into an array of booleans
    /// @param str The string to parse
    /// @param len Will be set to the length of the array
    /// @return The parsed array of booleans
    bool* parse_bool_list(const char* str, int& len);
    
    /// @brief Parses a string into an array of strings
    /// @param str The string to parse
    /// @param len Will be set to the length of the array
    /// @return The parsed array of strings
    char** parse_string_list(const char* str, int& len);
}


/**
 * @namespace ini_stringify
 * @brief Namespace for functions that stringify some types into `char*`
 */
namespace ini_stringify {
    /// @brief Stringifies a list of strings
    /// @param str_list The list to stringify
    /// @param len the length of the list
    /// @return The stringified list
    char* stringify_string_list(const char** str_list, int len);
}