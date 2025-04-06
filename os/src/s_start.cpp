#include "sbi.hpp"
#include "kernel_print.hpp"
#include "process.hpp"
#include "pmm.hpp"
#include "riscv.hpp"
#include "FS/MMIO.hpp"
#include "debug_utils.hpp"
#include "PLIC.hpp"
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

extern void smode_trap_handler(void);

void callback_1(void* p)
{
    kprint::printf("callback1: %p",p);
    return;
}


void hello(uint64 a0)
{
    kprint::putsln("Hello, HxcOS!");
    kprint::printf("a0=%p\n",a0);
    write_tp(a0);
    // print_memlayout();
    // kprint::printf("elf_begin=%p\n",elf_begin);
    // kprint::printf("elf_end=%p\n",elf_end);
    pmm::pmm_init();
    kprint::printf("pmm init complete\n");
}

extern "C" void s_start(uint64 a0)
{
    static int reach_s_start = 0;
    if(reach_s_start)
    {
        panic("You shouldn't reach here!\n");
    }
    reach_s_start = 1;
    hello(a0);

    
    
    write_csr(stvec, (uint64)smode_trap_handler);   // 设置我们的S-中断目标位置
    set_csr(sstatus, 1 << 1);
    set_csr(sie, 1 << 9);


    kprint::printf("plic stt\n");
    plicinit();
    plicinithart();
    plic_priority_init();
    plic_enable_interrupt();
    plic_threshold_init();

    kprint::printf("现在开始进行磁盘读取试验！\n");
    virtio_blk_init();

    panic("到这就算胜利\n");
    
    
    void* page1 = pmm::alloc_page();
    void* page2 = pmm::alloc_page();
    disk_request req = {
        .do_write = 0,
        .blkno = 1,
        .buffer = page1,
        .userdata = page2,
        .callback = callback_1
    };
    virtio_submit_request(&req);
    kprint::printf("请求已经发送\n");



    asm volatile("wfi");

    sbi::sbi_shutdown();
}