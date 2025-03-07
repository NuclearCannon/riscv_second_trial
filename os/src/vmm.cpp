#include "vmm.hpp"
#include "pmm.hpp"
#include "memlayout.hpp"
#include "assert.hpp"
#include "string.hpp"
#include "pmm.hpp"

// 对uint64的封装类，不可变，无额外空间
// PTE对外隐形！
class PageTableEntry
{
private:
    uint64 value;

public:
    PageTableEntry(void *pa, uint64 flag) : value(((uint64)pa >> 12) << 10 | (flag))
    {
        assert((uint64)pa % PGSIZE == 0);
    }
    PageTableEntry(const PageTableEntry &other) = default;
    PageTableEntry &operator=(const PageTableEntry &other) = default;
    void *getPa() const
    {
        return (void *)((value >> 10) << PGSHIFT);
    }
    uint64 getFlag() const
    {
        return value & 0x3FF;
    }
    bool isValid() const
    {
        return value & PTEFlags::V;
    }
    PageTableEntry *step(uint64 vpn) const
    {
        return (PageTableEntry *)getPa() + vpn;
    }
};

const int PTE_PER_PAGE = (PGSIZE / 8);
const PageTableEntry INVALID_PTE(NULL, PTEFlags::NONE);

static inline void *alloc_empty_page()
{
    return memset(pmm::alloc_page(), 0, PGSIZE);
}

// PageTable: Sv39

PageTable::PageTable()
{
    root = (PageTableEntry *)alloc_empty_page();
}

// 释放页表
PageTable::~PageTable()
{
    // 这可能需要一些考量……
    // 释放物理页这一动作到底谁来执行？
}

// 如果不存在这个键值对则return NULL
// 存在则返回对应页表项的指针
PageTableEntry *PageTable::page_walk(void *va) const
{
    uint64 addr = (uint64)va;
    addr >>= PGSHIFT;
    uint64 vpn0 = addr & 0x1FF;
    addr >>= 9;
    uint64 vpn1 = addr & 0x1FF;
    addr >>= 9;
    uint64 vpn2 = addr & 0x1FF;

    PageTableEntry *pte = root + vpn2;
    if (!(pte->isValid()))
        return NULL;
    pte = pte->step(vpn1);
    if (!(pte->isValid()))
        return NULL;
    pte = pte->step(vpn0);
    if (!(pte->isValid()))
        return NULL;
    return pte;
}

// 确保直到低级页表存在
// 返回在低级页表中的页表项的指针，但是并不保证其有效
PageTableEntry *PageTable::page_walk_alloc(void *va)
{
    uint64 addr = (uint64)va;
    addr >>= PGSHIFT;
    uint64 vpn0 = addr & 0x1FF;
    addr >>= 9;
    uint64 vpn1 = addr & 0x1FF;
    addr >>= 9;
    uint64 vpn2 = addr & 0x1FF;
    PageTableEntry *pte = root + vpn2;
    if (!(pte->isValid())) // 中间页表缺失
    {
        void *new_page = alloc_empty_page();          // 开辟中间页表
        *pte = PageTableEntry(new_page, PTEFlags::V); // 在根页表中补充中间页表项
        pte = (PageTableEntry *)new_page + vpn1;      //
        new_page = alloc_empty_page();                // 开辟低级页表
        *pte = PageTableEntry(new_page, PTEFlags::V); // 在中间页表中补充低级页表项
        return (PageTableEntry *)new_page + vpn0;     // 返回在低级页表中的页表项
    }
    pte = pte->step(vpn1); // 在中间页表中查找低级页表
    if (!(pte->isValid())) // 低级页表缺失
    {
        void *new_page = alloc_empty_page();
        *pte = PageTableEntry(new_page, PTEFlags::V);
        return (PageTableEntry *)new_page + vpn0;
    }
    return pte->step(vpn0);
}

int PageTable::add(void *va, void *pa, uint64 perm)
{
    PageTableEntry *pte = page_walk_alloc(va);
    if (pte->isValid())
        return -1;
    *pte = PageTableEntry(pa, perm);
    return 0;
}

void *PageTable::lookup(void *va) const
{
    PageTableEntry *pte = page_walk(va);
    if (pte == NULL)
        return NULL;
    return pte->getPa();
}

int PageTable::erase(void *va)
{
    PageTableEntry *pte = page_walk(va);
    if (pte == NULL)
        return -1;
    pmm::free_page(pte->getPa());
    *pte = INVALID_PTE;
    return 0;
}

uint64 PageTable::make_satp()
{
    static constexpr uint64_t SATP_SV39 = 8ULL << 60;
    return SATP_SV39 | (((uint64)(this->root)) >> 12);
}
