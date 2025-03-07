#include "sbi.hpp"
#include "kernel_print.hpp"
#include <stdarg.h>

using namespace kprint;

static const char hex_digits[] = "0123456789abcdef";  // 十六进制字符表

void kprint::putchar(char c)
{
    sbi::sbi_console_putchar(c);
}

void kprint::puts(const char* str) {
    while (*str) {          // 遍历字符串，直到遇到 '\0'
        putchar(*str);      // 输出当前字符
        str++;              // 移动到下一个字符
    }
}

void kprint::putsln(const char* str) {
    puts(str);
    putchar('\n');          // 输出换行符
}


// 打印一个 64 位整数（以十六进制形式）
void kprint::print_p(void* ptr) {
    // 输出 "0x" 前缀
    putchar('0');
    putchar('x');
    // 将指针值逐位转换为十六进制字符
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = ((uint64)(ptr) >> i) & 0xF;  // 每次取 4 位
        putchar(hex_digits[nibble]);
        
    }
}


void kprint::printf(const char* format, ...) {
    va_list args;            // 可变参数列表
    va_start(args, format); // 初始化可变参数

    while (*format) {       // 遍历格式化字符串
        if (*format == '%') { // 如果遇到 '%'
            format++;       // 移动到格式说明符
            switch (*format) {
                case 'c': {  // 字符
                    char c = va_arg(args, int); // 获取字符参数
                    putchar(c);
                    break;
                }
                case 's': {  // 字符串
                    const char* str = va_arg(args, const char*); // 获取字符串参数
                    puts(str);
                    break;
                }
                case 'd': {  // 整数
                    int num = va_arg(args, int); // 获取整数参数
                    if (num < 0) {              // 处理负数
                        putchar('-');
                        num = -num;
                    }
                    char buffer[12];            // 缓冲区，用于存储数字字符
                    int i = 0;
                    do {
                        buffer[i++] = '0' + (num % 10); // 将数字转换为字符
                        num /= 10;
                    } while (num > 0);
                    while (i > 0) {
                        putchar(buffer[--i]);  // 逆序输出字符
                    }
                    break;
                }
                case 'p': {  // 输出 十六进制
                    void* num = va_arg(args, void*);
                    print_p(num); 
                    break;
                }
                case '%': {  // 输出 '%'
                    putchar('%');
                    break;
                }
                default:     // 未知格式说明符
                    putchar('%');
                    putchar(*format);
                    break;
            }
        } else {            // 普通字符
            putchar(*format);
        }
        format++;           // 移动到下一个字符
    }

    va_end(args);           // 结束可变参数
}