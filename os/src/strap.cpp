#include "debug_utils.hpp"
#include "types.hpp"
#include "riscv.hpp"


void smode_trap_handler(void) __attribute__((aligned(4)));
//
// kernel/smode_trap.S will pass control to smode_trap_handler, when a trap happens
// in S-mode.
//
void smode_trap_handler(void) {
    panic("成功进入！");


    uint64 tp = read_tp();
    uint64 epc = read_csr(sepc);
    uint64 cause = read_csr(scause);

    kprint::printf("smode_trap_handler: tp=%p,epc=%p,cause=%p\n",tp,epc,cause);

    panic("smode_trap_handler还未实现！");


}
