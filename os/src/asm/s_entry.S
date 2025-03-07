# .option pic
.text
.section .text.entry
.globl _entry
.extern s_start
# 从BIOS转到这里时是S态
_entry:
    la sp, stack0+4096   # 初始化内核栈
    csrw satp, zero         # 设置Bare Mode
    sfence.vma zero, zero
    call s_start            # 跳转到s_start函数进行进一步操作


# 在此处定义内核栈！
.align 12
stack0:
.space 4096
