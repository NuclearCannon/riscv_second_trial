#pragma once
#include "types.hpp"

namespace sbi
{



#define SBI_CONSOLE_PUTCHAR 0x1L
#define SBI_CONSOLE_GETCHAR 0x2L
#define SBI_HSM_EID 0x48534D
#define SBI_RESET_EID 0x53525354
#define SBI_TIMER_EID 0x54494D45
#define SBI_CALL(ext, funct, arg0, arg1, arg2, arg3)                                                                   \
    ({                                                                                                                 \
        register uintptr_t a0 asm("a0") = (uintptr_t) (arg0);                                                          \
        register uintptr_t a1 asm("a1") = (uintptr_t) (arg1);                                                          \
        register uintptr_t a2 asm("a2") = (uintptr_t) (arg2);                                                          \
        register uintptr_t a3 asm("a3") = (uintptr_t) (arg3);                                                          \
        register uintptr_t a6 asm("a6") = (uintptr_t) (funct);                                                         \
        register uintptr_t a7 asm("a7") = (uintptr_t) (ext);                                                           \
        asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a1), "r"(a2), "r"(a3), "r"(a6), "r"(a7) : "memory");           \
        (struct sbiret){a0, a1};                                                                                       \
    })

#define SBI_CALL_0(ext, funct) SBI_CALL(ext, funct, 0, 0, 0, 0)
#define SBI_CALL_1(ext, funct, arg0) SBI_CALL(ext, funct, arg0, 0, 0, 0)
#define SBI_CALL_2(ext, funct, arg0, arg1) SBI_CALL(ext, funct, arg0, arg1, 0, 0)
#define SBI_CALL_3(ext, funct, arg0, arg1, arg2) SBI_CALL(ext, funct, arg0, arg1, arg2, 0)
#define SBI_CALL_4(ext, funct, arg0, arg1, arg2, arg3) SBI_CALL(ext, funct, arg0, arg1, arg2, arg3)

struct sbiret {
    uint64 error;
    uint64 value;
};

// system reset
static inline struct sbiret sbi_system_reset(uint32 reset_type, uint32 reset_reason) {
    return SBI_CALL_2(SBI_RESET_EID, 0, reset_type, reset_reason);
}

// set timer
static inline struct sbiret sbi_set_timer(uint64 stime_value) {
    // stime_value is in absolute time.
    return SBI_CALL_1(SBI_TIMER_EID, 0, stime_value);
}

// shutdown
static inline struct sbiret sbi_shutdown() { return sbi_system_reset(0, 0); }

// putchar
static inline void sbi_console_putchar(int ch) { SBI_CALL_1(SBI_CONSOLE_PUTCHAR, 0, ch); }

// getchar
static inline int sbi_console_getchar() { return SBI_CALL_0(SBI_CONSOLE_GETCHAR, 0).error; }

// hart start
static inline struct sbiret sbi_hart_start(uint64 hartid, uint64 start_addr, uint64 opaque) {
    return SBI_CALL_3(SBI_HSM_EID, 0, hartid, start_addr, opaque);
}

// hart stop
static inline struct sbiret sbi_hart_stop(uint64 hartid) { return SBI_CALL_0(SBI_HSM_EID, 1); }
    
}; // namespace sbi