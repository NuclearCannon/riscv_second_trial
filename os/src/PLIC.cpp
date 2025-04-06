#include "PLIC.hpp"
#include "types.hpp"
#include "kernel_print.hpp"
#include "riscv.hpp"
#include "debug_utils.hpp"

#define writeb(v, addr) \
    { (*(volatile uint8 *)(addr)) = (v); }
#define writew(v, addr) \
    { (*(volatile uint16 *)(addr)) = (v); }
#define writed(v, addr) \
    { (*(volatile uint32 *)(addr)) = (v); }
#define writeq(v, addr) \
    { (*(volatile uint64 *)(addr)) = (v); }



// #ifdef QEMU  // QEMU
#define UART_IRQ 10
#define DISK_IRQ 1
// #else  // k210
// #define UART_IRQ 33
// #define DISK_IRQ 27
// #endif

void plicinit(void) {
    kprint::printf("enter plic\n");
    writed(1, PLIC_V + DISK_IRQ * sizeof(uint32));
    kprint::printf("mid plic\n");
    writed(1, PLIC_V + UART_IRQ * sizeof(uint32));


    kprint::printf("plicinit\n");

}

void plicinithart(void) 
{
    
    uint64 hart = read_tp();
    kprint::printf("enter plichart, hartid=%p\n",hart);
    if(hart!=0)
    {
        panic("hart!=0 ???");
    }
    
// #ifdef QEMU
    // set uart's enable bit for this hart's S-mode.
    *(uint32*)PLIC_SENABLE(hart) = (1 << UART_IRQ) | (1 << DISK_IRQ);
    // set this hart's S-mode priority threshold to 0.
    *(uint32*)PLIC_SPRIORITY(hart) = 0;
// #else
//     uint32 *hart_m_enable = (uint32 *)PLIC_MENABLE(hart);
//     *(hart_m_enable) = readd(hart_m_enable) | (1 << DISK_IRQ);
//     uint32 *hart0_m_int_enable_hi = hart_m_enable + 1;
//     *(hart0_m_int_enable_hi) =
//         readd(hart0_m_int_enable_hi) | (1 << (UART_IRQ % 32));
// #endif
// #ifdef DEBUG
    kprint::printf("plicinithart\n");
// #endif
}

// ask the PLIC what interrupt we should serve.
int plic_claim(void) {
    int hart = read_tp();
    int irq;
// #ifndef QEMU
    irq = *(uint32*)PLIC_MCLAIM(hart);
// #else
    // irq = *(uint32 *)PLIC_SCLAIM(hart);
// #endif
    return irq;
}

// tell the PLIC we've served this IRQ.
void plic_complete(int irq) {
    int hart = read_tp();
// #ifndef QEMU
    *(uint32*)PLIC_MCLAIM(hart) = irq;
// #else
    // *(uint32 *)PLIC_SCLAIM(hart) = irq;
// #endif
}

void plic_priority_init(void)
{
    
    volatile uint32_t *plic_priority = (volatile uint32_t *)(PLIC_PRIORITY + 4 * 8);
    *plic_priority = 7; // 设置 IRQ 8 的优先级为 7（最高）
}



void plic_enable_interrupt()
{
    #define PLIC_ENABLE_BASE 0x0c002000
    volatile uint32_t *plic_enable = (volatile uint32_t *)(PLIC_ENABLE_BASE + 0x80 * 0);
    *plic_enable |= (1 << 8); // 启用 IRQ 8
}

void plic_threshold_init()
{
    #define PLIC_THRESHOLD_BASE 0x0c200000
    volatile uint32_t *plic_threshold = (volatile uint32_t *)(PLIC_THRESHOLD_BASE + 0x1000 * 0);
    *plic_threshold = 0; // 阈值为 0
}