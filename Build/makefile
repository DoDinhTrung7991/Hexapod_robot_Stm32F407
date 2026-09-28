#----------	INCLUDE_START	----------#
include prj_info.mk

# Find all module.mk files and include them.
MODULE_MK_FILES := $(shell find $(SRC_PATH) -name 'module.mk')
-include $(MODULE_MK_FILES)
#----------	INCLUDE_END		----------#

#----------	MACRO_START	----------#
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Compiler flags
COMMON_CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -std=gnu11 -ffunction-sections -fdata-sections -ffreestanding -Wall
CFLAGS = -O2 $(COMMON_CFLAGS)
CFLAGS_GDB = -O0 -ggdb $(COMMON_CFLAGS)

# Linker flags
LDFLAGS_COMMON = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -nostdlib -lgcc -Wall -Wl,--gc-sections
LDFLAGS = $(LDFLAGS_COMMON) -T $(SRC_PATH)Linker/linker.ld -Wl,-Map=$(BUILD_RESULT_PATH)final.map
LDFLAGS_GDB = $(LDFLAGS_COMMON) -T $(SRC_PATH)Linker/linker.ld -Wl,-Map=$(BUILD_RESULT_PATH)final_gdb.map -ggdb

# Automatically find all .c files in the source directory
SRC_FILES := $(shell find $(SRC_PATH) -name '*.c')

# Generate object file lists from source files
OBJ_FILES = $(patsubst $(SRC_PATH)%.c,$(BUILD_RESULT_PATH)%.o,$(SRC_FILES))
OBJ_FILES_GDB = $(patsubst $(SRC_PATH)%.c,$(BUILD_RESULT_PATH)%_gdb.o,$(SRC_FILES))

# Gather all _INC variables from module.mk files into a single list
ALL_INC_VARS := $(filter %_INC,$(.VARIABLES))
ALL_INC_PATHS := $(sort $(foreach var,$(ALL_INC_VARS),$($(var))))

# VPATH for source files
VPATH = $(SRC_PATH)
#----------	MACRO_END	----------#


#----------	TARGETS_START	----------#
.PHONY: clean all all_gdb flash debug

# Default target
all: $(BUILD_RESULT_PATH)Hexapod_Robot.elf
	@printf "Build successful!!!\r\n\r\n"

all_gdb: $(BUILD_RESULT_PATH)Hexapod_Robot_gdb.elf
	@printf "Debug build successful!!!\r\n\r\n"

# Rule to link the final ELF file
$(BUILD_RESULT_PATH)Hexapod_Robot.elf: $(OBJ_FILES)
	@echo "Linking..."
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJCOPY) -O binary $@ $(BUILD_RESULT_PATH)Hexapod_Robot.bin
	@echo "ELF file and BIN file created: $@"
	@$(SIZE) $@
	@printf "\r\n"

$(BUILD_RESULT_PATH)Hexapod_Robot_gdb.elf: $(OBJ_FILES_GDB)
	@echo "Linking (GDB)..."
	$(CC) $(LDFLAGS_GDB) -o $@ $^
	@echo "ELF file for GDB created: $@"
	@$(SIZE) $@
	@printf "\r\n"

# Pattern rule for building objects
$(BUILD_RESULT_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(ALL_INC_PATHS) -c $< -o $@
	@printf "\r\n"

# Pattern rule for building debug objects
$(BUILD_RESULT_PATH)%_gdb.o: $(SRC_PATH)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< (GDB)..."
	$(CC) $(CFLAGS_GDB) $(ALL_INC_PATHS) -c $< -o $@
	@printf "\r\n"

#----------	UTILITY_TARGETS_START	----------#
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_RESULT_PATH)*

erase:
	st-flash erase

flash: all
	st-flash write $(BUILD_RESULT_PATH)Hexapod_Robot.bin 0x08000000

debug: all_gdb
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg
#----------	UTILITY_TARGETS_END	----------#
