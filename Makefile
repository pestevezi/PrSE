PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -Og -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I./includes -I./drivers $(COMMONFLAGS)
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
RM=rm -f


SRC=$(wildcard *.c)
SRC_LED=$(filter-out hello_world.c, $(filter-out pin_mux_hello.c , $(SRC)))
SRC_HELLO=$(filter-out led_blinky.c, $(filter-out pin_mux.c , $(SRC)))

OBJ_LED=$(patsubst %.c, %.o, $(SRC_LED))
OBJ_HELLO=$(patsubst %.c, %.o, $(SRC_HELLO))

led_blinky.elf: $(OBJ_LED)
	$(LD) $(LDFLAGS) $(OBJ_LED) $(LDLIBS) -o $@

hello_world.elf: $(OBJ_HELLO)
	$(LD) $(LDFLAGS) $(OBJ_HELLO) $(LDLIBS) -o $@
