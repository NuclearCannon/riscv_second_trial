#pragma once

// #define VIRT_OFFSET 0x3F00000000L
#define VIRT_OFFSET 0x00000000L

#define PLIC 0x0c000000L
#define PLIC_V (PLIC + VIRT_OFFSET)

#define PLIC_PRIORITY (PLIC_V + 0x0)
#define PLIC_PENDING (PLIC_V + 0x1000)
#define PLIC_MENABLE(hart) (PLIC_V + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC_V + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC_V + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC_V + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC_V + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC_V + 0x201004 + (hart)*0x2000)


void plicinit(void);
void plicinithart(void);
int plic_claim(void);
void plic_complete(int irq);

void plic_priority_init(void);
void plic_enable_interrupt();
void plic_threshold_init();