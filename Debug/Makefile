# Cross compiling:
CROSS_COMPILE = arm-none-eabi-

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
SIZE			= $(CROSS_COMPILE)size

INCLUDES_DIRS = $(sort $(dir $(wildcard ../includes/*/)))
INCLUDES_FILES := $(foreach dir,$(INCLUDES_DIRS),-I"$(dir)")

# TODO: test with O3
OPT_LVL = 0

# Using thumb for version 7 ARM core:
CFLAGS  = -mcpu=cortex-m4 -march=armv7e-m -mthumb -g -Wall -ffreestanding -O$(OPT_LEVEL) -Og -fdata-sections -ffunction-sections $(INCLUDES_FILES) -DSTM32F429_439xx
ASFLAGS = -mcpu=cortex-m4 -march=armv7e-m -mthumb -g -Wall -ffreestanding -O$(OPT_LEVEL) -Og -fdata-sections -ffunction-sections

# Use our own linker script
LDFLAGS = -L"../ldscripts/" -T ldmain.lds -nostartfiles -nostdlib --gc-sections --print-gc-sections

SRCDIR = ../src
OUTDIR = ../Debug
TARGET = bootloader

# Source files and objects
TSRC = $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c)
TOBJ = $(TSRC:.c=.o)
TSRC2 = $(wildcard $(SRCDIR)/*.S $(SRCDIR)/*/*.S)
TOBJ2 = $(TSRC2:.S=.o)
TSRC3 = $(wildcard ../includes/*.h ../includes/*/*.h)
OBJS = $(patsubst $(SRCDIR)/%, $(OUTDIR)/%,$(TOBJ))
OBJS2 = $(patsubst $(SRCDIR)/%, $(OUTDIR)/%,$(TOBJ2))


all: $(TARGET).bin
	$(SIZE) $(TARGET)
	@$(OBJDUMP) $(TARGET) -D > disassembled_D.txt
	@$(OBJDUMP) $(TARGET) -S > disassembled_S_Big.txt
	@$(OBJDUMP) $(TARGET) -s > disassembled_s_little.txt

$(TARGET).bin: $(TARGET)
	@$(OBJCOPY) -O binary $^ $@
	@$(OBJCOPY) -O ihex $(TARGET) $(TARGET).hex
	@$(OBJCOPY) -O elf32-littlearm $(TARGET) $(TARGET).elf

$(TARGET): $(OBJS) $(OBJS2)
	@$(LD) $(LDFLAGS) $^ -o $(OUTDIR)/$@
	
$(OUTDIR)/%.o: $(SRCDIR)/%.c $(TSRC3)
	@mkdir -p $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

$(OUTDIR)/%.o: $(SRCDIR)/%.S $(TSRC3)
	@mkdir -p $(@D)
	@$(CC) -c -o $@ $< $(ASFLAGS)

clean:
	@mkdir -p $(OUTDIR)/_dummy/
	@rm -f $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET) $(OUTDIR)/*.txt $(OUTDIR)/*.o $(OUTDIR)/*~ $(OBJS) $(OBJS2)
	@rm -r $(OUTDIR)/*/

