#pragma once


#define RAM_BASE 0x80000000ULL
#define RAM_SHIFT ((uint64)(7+10+10)) //128MB
#define RAM_SIZE (1<<RAM_SHIFT)  // 128MB内存
#define RAM_END (RAM_BASE+RAM_SIZE)


#define KERNEL_BEGIN  0x80200000ULL
extern "C" {
    extern char _kernel_end[];          // 标记内核的结束
}
#define KERNEL_END ((uint64)_kernel_end)

#define PGSHIFT 12
#define PGSIZE 4096