#pragma once

namespace pmm {
    void* alloc_page();
    void free_page(void* page);
    void pmm_init();
}