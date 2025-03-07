# 这个文件用于一键编译+运行！
cd os
make clean
make
cd ..
# qemu-system-riscv64 -machine virt -nographic -m 128M -kernel os/bin/kernel -bios opensbi.elf -d in_asm,cpu 2> ./log
qemu-system-riscv64 -machine virt -nographic -m 128M -kernel os/bin/kernel -bios BIOS/opensbi.elf