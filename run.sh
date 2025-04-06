# 这个文件用于一键编译+运行！
cd os
make clean
make
cd ..
# qemu-system-riscv64 -machine virt -nographic -m 128M -kernel os/bin/kernel -bios opensbi.elf -d in_asm,cpu 2> ./log
qemu-system-riscv64 \
    -machine virt \
    -nographic \
    -m 128M \
    -kernel os/bin/kernel \
    -bios BIOS/opensbi.elf \
    -drive file=sdcard-riscv64.img,if=none,format=raw,id=x0  \
	-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 # -d in_asm,cpu -D qemu.log

