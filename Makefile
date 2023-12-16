#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -Og -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I./includes -I./drivers $(COMMONFLAGS)
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

TARGET=main

SRC=$(wildcard *.c dotcs/*.c)
OBJ=$(patsubst %.c, %.o, $(SRC)) #divide.o

all: build size
build: elf srec bin
elf: $(TARGET).elf
srec: $(TARGET).srec
bin: $(TARGET).bin

clean:
	$(RM) $(TARGET).srec $(TARGET).elf $(TARGET).bin $(TARGET).map $(OBJ)

$(TARGET).elf: $(OBJ)
#	arm-none-eabi-as -mthumb -mcpu=cortex-m0plus -o divide.o divide.s
	$(LD) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

size:
	$(SIZE) $(TARGET).elf

flash: all
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"