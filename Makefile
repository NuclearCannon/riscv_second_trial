.PHONY: all clean

KERNEL := os/bin/kernel
BIOS := BIOS/opensbi.elf
QEMU := qemu-system-riscv64
GDB := riscv64-unknown-elf-gdb

# qemu参数
QEMU_ARGS := \
    -machine virt \
    -nographic \
    -m 128M \
    -kernel $(KERNEL) \
    -bios $(BIOS) \
    -drive file=sdcard-riscv64.img,if=none,format=raw,id=x0 \
    -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

all:
	$(MAKE) -C os

clean:
	$(MAKE) -C src clean

run:
	$(MAKE) -C os clean
	$(MAKE) -C os
	$(QEMU) $(QEMU_ARGS)

debug:
	$(MAKE) -C os clean
	$(MAKE) -C os
	@echo "启动 QEMU（等待 GDB 连接）..."
	$(QEMU) $(QEMU_ARGS) -s -S
	
gdb:
	$(GDB) -q -ex "file $(KERNEL)" \
		-ex "add-symbol-file $(KERNEL) 0x80200000" \
		-ex "target remote :1234" \
		-ex "b s_start"  # 在 main 函数处设置断点

