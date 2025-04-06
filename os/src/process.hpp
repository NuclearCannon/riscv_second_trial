#pragma once

#include "types.hpp"
#include "vmm.hpp"

class RiscvRegs
{
public:
    /*  0  */ uint64 ra;
    /*  8  */ uint64 sp;
    /*  16 */ uint64 gp;
    /*  24 */ uint64 tp;
    /*  32 */ uint64 t0;
    /*  40 */ uint64 t1;
    /*  48 */ uint64 t2;
    /*  56 */ uint64 s0;
    /*  64 */ uint64 s1;
    /*  72 */ uint64 a0;
    /*  80 */ uint64 a1;
    /*  88 */ uint64 a2;
    /*  96 */ uint64 a3;
    /* 104 */ uint64 a4;
    /* 112 */ uint64 a5;
    /* 120 */ uint64 a6;
    /* 128 */ uint64 a7;
    /* 136 */ uint64 s2;
    /* 144 */ uint64 s3;
    /* 152 */ uint64 s4;
    /* 160 */ uint64 s5;
    /* 168 */ uint64 s6;
    /* 176 */ uint64 s7;
    /* 184 */ uint64 s8;
    /* 192 */ uint64 s9;
    /* 196 */ uint64 s10;
    /* 208 */ uint64 s11;
    /* 216 */ uint64 t3;
    /* 224 */ uint64 t4;
    /* 232 */ uint64 t5;
    /* 240 */ uint64 t6;
};

class TrapFrame {
    public:
    // space to store context (all common registers)
    /* offset:0   */ RiscvRegs regs;
  
    // process's "user kernel" stack
    /* offset:248 */ uint64 kernel_sp;
    // pointer to smode_trap_handler
    /* offset:256 */ uint64 kernel_trap;
    // saved user process counter
    /* offset:264 */ uint64 epc;
    // kernel page table. added @lab2_1
    /* offset:272 */ uint64 kernel_satp;
};

class MappedInfo
{
public:
    uint64 code_addr;
    uint64 code_page_num;
    uint64 data_addr;
    uint64 data_page_num;
    uint64 stack_page_num;  // 栈没有基地址
};

class Process
{
private:
    int pid;
    TrapFrame* trapframe;
    MappedInfo mapped_info;
    PageTable page_table;
public:
    Process(int pid);
    ~Process();

    void load_from_elf(const char* elf_buff);

};