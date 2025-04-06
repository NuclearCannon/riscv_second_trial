#include "process.hpp"
#include "kernel_print.hpp"
#include "elf.hpp"
#include "string.hpp"
#include "assert.hpp"
#include "pmm.hpp"

Process::Process(int pid):pid(pid)
{
    kprint::printf("process %d inited\n", pid);
}

Process::~Process()
{
    kprint::printf("process %d deconstructed\n", pid);
    pid = 0;
}

uint64 PAGE_NUMBER(uint64 addr)
{
    return addr>>12;
}



void Process::load_from_elf(const char* elf_buff)
{
    // 读取ELF头
    struct elf_header_t elf_header;
    memcpy(&elf_header, elf_buff, sizeof(elf_header));

    // 检查ELF魔数
    assert(elf_header.magic == ELF_MAGIC);

    // 加载ELF段
    elf_prog_header segment_header;  // 临时存放段头
    // 遍历段头表 
    assert((elf_header.phnum <= 2));
    for (int i = 0, off = elf_header.phoff; i < elf_header.phnum; i++, off += sizeof(segment_header))
    {
        // 读取段头
        memcpy(&segment_header, elf_buff+off, sizeof(segment_header));

        // elf_fpread(f, (void *)&segment_header, sizeof(segment_header), off);
        if (segment_header.type != ELF_PROG_LOAD) continue;
        // 这里删去了很多检查ELF文件内容是否合法，不合法就panic的内容

        // 需要计算这个段需要使用多少物理页
        uint64 va_stt, va_end, pages_need;
        va_stt = PAGE_NUMBER(segment_header.vaddr);
        va_end = PAGE_NUMBER(segment_header.vaddr + segment_header.memsz - 1);
        pages_need = (va_end-va_stt)/PGSIZE+1;
        uint64 perm;
        if( segment_header.flags == (SEGMENT_READABLE|SEGMENT_EXECUTABLE) )
        {
            mapped_info.code_addr = va_stt;
            mapped_info.code_page_num = pages_need;
            perm = permission_code(0,1,1,1,0);
            
        }
        else if ( segment_header.flags == (SEGMENT_READABLE|SEGMENT_WRITABLE) )
        {
            mapped_info.data_addr = va_stt;
            mapped_info.code_addr = pages_need;
            perm = permission_code(1,1,0,1,0);
        }
        else
        {
            assert(0);
        }
        // 分配物理页
        uint64 remaining_size = segment_header.memsz;
        for(int j=0;j<pages_need;j++)
        {
            assert(remaining_size>0);
            uint64 va = va_stt + j*PGSIZE;
            uint64 nb = remaining_size > PGSIZE ? PGSIZE : remaining_size;

            void* dest = pmm::alloc_page();
            page_table.add((void*)va, dest, perm);
            
            memcpy(dest, (void*)(segment_header.off + j*PGSIZE), nb);
            remaining_size -= PGSIZE;
        }
    }
    trapframe->epc = elf_header.entry;
    // 这里缺少了初始化gp寄存器的内容！
}