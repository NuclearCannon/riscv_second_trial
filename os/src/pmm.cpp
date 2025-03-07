#include "memlayout.hpp"
#include "types.hpp"
#include "kernel_print.hpp"
#include "pmm.hpp"
#include "assert.hpp"

using namespace pmm;

static void* free_pages_list;




// 物理页追踪系统尚不使用
// class PhysicalPageDescriptor {
//     uint32 ref_cnt;
// };

// static PhysicalPageDescriptor ppds[1<<(RAM_SHIFT-PGSHIFT)];

// static const int sizeof_ppds = sizeof(ppds);

// static PhysicalPageDescriptor& get_ppd(void* pa)
// {
//     assert((uint64)pa % PGSIZE==0);
//     uint64 idx = ((uint64)pa - RAM_BASE)>>PGSHIFT;
//     return ppds[idx];
// }

void* pmm::alloc_page()
{
    void* p = free_pages_list;
    if(p)free_pages_list = *(void**)p;
    return p;
}



void pmm::free_page(void* page)
{
    *(void**)page = free_pages_list;
    free_pages_list = page;
}

void pmm::pmm_init()
{
    kprint::printf("KERNEL_END=%p\n",KERNEL_END);
    kprint::printf("RAM_END=%p\n",RAM_END);
    int num = 0;
    for(void* p=(void*)KERNEL_END; p<(void*)RAM_END; p+=PGSIZE)
    {
        free_page(p);
        num++;
    }
    kprint::printf("%d pages inserted\n",num);
}