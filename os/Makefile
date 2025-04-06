# 该文件用于将os/src中的内容编译为内核，存放在os/bin中，文件名为kernel，文件格式为ELF

# 编译器设置
CC = riscv64-unknown-elf-gcc
CPPC = riscv64-unknown-elf-g++
AS = riscv64-unknown-elf-as
LD = riscv64-unknown-elf-ld
OBJCOPY = riscv64-unknown-elf-objcopy

# 编译选项
CFLAGS = -g -Wall -march=rv64imac -mabi=lp64 -ffreestanding -nostdlib -mcmodel=medany
CXXFLAGS = $(CFLAGS) -std=c++17  # C++ 编译选项
CXXFLAGS += -fno-exceptions		# 禁用：异常控制流
ASFLAGS = -march=rv64imac -mabi=lp64# -mcmodel=medany
LDFLAGS = -T ./linker.ld -nostdlib

# 源文件和目标文件
SRC_DIR = ./src
BIN_DIR = ./bin

# 递归查找所有.c和.S文件
SRC_C = $(shell find $(SRC_DIR) -name '*.c')
SRC_CPP = $(shell find $(SRC_DIR) -name '*.cpp')
SRC_S = $(shell find $(SRC_DIR) -name '*.S')

# 将.c和.S文件路径替换为.o文件路径
OBJ_C = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_C))
OBJ_CPP = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SRC_CPP))
OBJ_S = $(patsubst $(SRC_DIR)/%.S, $(BIN_DIR)/%.o, $(SRC_S))

# 目标文件
TARGET = $(BIN_DIR)/kernel

# 默认目标
all: $(TARGET)

# 编译C文件
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)  # 确保目标目录存在
	$(CC) $(CFLAGS) -c $< -o $@

# 编译C++文件
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)  # 确保目标目录存在
	$(CPPC) $(CXXFLAGS) -c $< -o $@

# 编译汇编文件
$(BIN_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)  # 确保目标目录存在
	$(AS) $(ASFLAGS) -c $< -o $@

# 链接目标文件生成ELF文件
$(TARGET): $(OBJ_C) $(OBJ_CPP) $(OBJ_S) 
	$(LD) $(LDFLAGS) $^ -o $@

# 清理生成的文件
clean:
	rm -rf $(BIN_DIR)/*.o $(TARGET)

.PHONY: all clean