#include "sbi.hpp"
#include "kernel_print.hpp"
#include "process.hpp"
#include "pmm.hpp"
extern const char elf_begin[];
extern const char elf_end[];


extern "C" {
    extern char _text_begin[];
    extern char _text_end[];
    extern char _rodata_begin[];
    extern char _rodata_end[];
    extern char _data_begin[];
    extern char _data_end[];
    extern char _bss_begin[];
    extern char _bss_end[];

};

void print_memlayout()
{
    kprint::printf("_text_begin=   %p\n",_text_begin);
    kprint::printf("_text_end=     %p\n",_text_end);
    kprint::printf("_rodata_begin= %p\n",_rodata_begin);
    kprint::printf("_rodata_end=   %p\n",_rodata_end);
    kprint::printf("_data_begin=   %p\n",_data_begin);
    kprint::printf("_data_end=     %p\n",_data_end);
    kprint::printf("_bss_begin=    %p\n",_bss_begin);
    kprint::printf("_bss_end=      %p\n",_bss_end);

}

extern "C" void s_start()
{
    static int reach_s_start = 0;
    if(reach_s_start)
    {
        kprint::putsln("You shouldn't reach here!\n");
        sbi::sbi_shutdown();
    }
    reach_s_start = 1;
    //char str[50] = "Hello, HxcOS!";
    kprint::putsln("Hello, HxcOS!");
    print_memlayout();
    kprint::printf("elf_begin=%p\n",elf_begin);
    kprint::printf("elf_end=%p\n",elf_end);
    pmm::pmm_init();
    kprint::printf("pmm init complete");

    {
        Process p(123);
    }
    void* page = pmm::alloc_page();
    kprint::printf("page = %p\n",page);
    sbi::sbi_shutdown();
}