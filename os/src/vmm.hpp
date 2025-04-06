#pragma once

// Sv39页表机制！
#include "types.hpp"

namespace PTEFlags {
    static constexpr uint64 NONE = 0;
    static constexpr uint64 V = 1ULL << 0;  // valid
    static constexpr uint64 R = 1ULL << 1;  // readable
    static constexpr uint64 W = 1ULL << 2;  // writable
    static constexpr uint64 X = 1ULL << 3;  // executable
    static constexpr uint64 U = 1ULL << 4;  // user accessible
    static constexpr uint64 G = 1ULL << 5;  // global
    static constexpr uint64 A = 1ULL << 6;  // accessed
    static constexpr uint64 D = 1ULL << 7;  // dirty
    static constexpr uint64 COW = 1ULL << 8; // copy-on-write
}

constexpr uint64 permission_code(bool write, bool read, bool exec, bool user, bool cow)
{
    uint64 perm = PTEFlags::V;
    if(write)perm |= PTEFlags::W;
    if(read)perm |= PTEFlags::R;
    if(exec)perm |= PTEFlags::X;
    if(user)perm |= PTEFlags::U;
    if(cow)perm |= PTEFlags::COW;
    return perm;
}


class PageTableEntry;   // 仅声明

class PageTable
{
private:
    
    PageTableEntry* page_walk(void* va) const;
    PageTableEntry* page_walk_alloc(void* va);
    PageTableEntry* root;
public:
    // 构造一个空页表
    PageTable();
    // 释放页表
    ~PageTable();
    // 向页表中加入键值对，正常返回0，如果已经存在键为va的键值对return -1
    int add(void* va, void* pa, uint64 perm);
    // 查询，查询失败return NULL
    void* lookup(void* va) const;
    // 删除键值对，正常返回0，如果原本就不存在return -1
    // 这会对这个物理页进行一次free_page！
    int erase(void* va);
    uint64 make_satp();
};

constexpr uint64 PGSIZE = 4096;