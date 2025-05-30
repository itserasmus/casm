#pragma once

#include "col_cout.hpp"
#include "settings.hpp"
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>

#include <chrono>

using namespace std;

void read_from_pipe(HANDLE pipe, bool is_error, chrono::steady_clock::time_point start_time) {
    CHAR buffer[4096];
    DWORD bytesRead;
    string line_buffer;

    while(true) {
        BOOL success = ReadFile(pipe, buffer, sizeof(buffer)-1, &bytesRead, NULL);
        if(!success || bytesRead == 0) break; // error or stream over

        buffer[bytesRead] = '\0';
        line_buffer += buffer;

        size_t pos;
        while((pos = line_buffer.find('\n')) != string::npos) {
            std::string line = line_buffer.substr(0, pos);
            line_buffer.erase(0, pos + 1);

            auto curr_time = chrono::steady_clock::now();
            int ms = (int)chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time).count();

            printf_mtx.lock();
            if(is_error) {
                printf("\u001b[31m%5lld | %s\n\u001b[0m", chrono::duration_cast<chrono::milliseconds>(curr_time-start_time).count(), line.c_str());
            } else {
                printf("%5lld | %s\n", chrono::duration_cast<chrono::milliseconds>(curr_time-start_time).count(), line.c_str());
            }
            printf_mtx.unlock();
        }
    }
}

void heavy_profile(string file_path) {
    colout << "Heavy Profiling " << BR_CYAN << file_path << RESET << "...\n" << newline_split;
    
    HANDLE out_read, out_write, err_read, err_write;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // create pipes to redirect out & err
    CreatePipe(&out_read, &out_write, &sa, 0);
    SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&err_read, &err_write, &sa, 0);
    SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(STARTUPINFOA) }; // what is a startup infoa who even names these things
    PROCESS_INFORMATION pi;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = out_write;
    si.hStdError = err_write;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    string cmd_line = string("\"") + file_path + "\"";
    
    auto start_time = chrono::steady_clock::now();

    BOOL success = CreateProcessA(
        NULL, (LPSTR)cmd_line.c_str(),
        NULL, NULL, TRUE, // why, microsoft, why
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi);
    
    if (!success) {
        colout << "Failed to launch process, error: " << GetLastError() << "\n";
        return;
    }

    CloseHandle(out_write); // idk why but this is needed
    CloseHandle(err_write);

    thread t_out(read_from_pipe, out_read, false, start_time);
    thread t_err(read_from_pipe, err_read, true, start_time);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    t_out.join();
    t_err.join();

    CloseHandle(out_read);
    CloseHandle(err_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    auto end_time = chrono::steady_clock::now();

    colout << newline_split;
    if(exit_code != 0) {
        colout << RED << "Execution failed with error code: " << exit_code << RESET;
    } else {
        colout << GREEN << "Execution successful " << RESET;
    }
    colout << CYAN << "(" << chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count() << "ms)\n" << RESET;
}
