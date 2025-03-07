# .option pic
.data
.globl elf_begin
.globl elf_end

elf_begin:
.incbin "bin/include_files/example"      # 这里填写你要include进去的ELF文件！
elf_end: