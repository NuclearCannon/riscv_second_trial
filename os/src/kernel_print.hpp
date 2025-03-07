#pragma once


namespace kprint {

    void putchar(char c);
    void puts(const char* str);
    void putsln(const char* str);
    void print_p(void* ptr);
    void printf(const char* format, ...);

}