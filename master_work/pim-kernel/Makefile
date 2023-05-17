CC := gcc
override CFLAGS += -Ofast -Wall -static -nostdlib

SOURCE := pim-kernel.c
ifdef ARGS
ifeq ($(ARGS), DPIM0)
BINARY := pim-kernel-0 # pim_system:0
START_ADDR := 0x440000000 # must match the memory start address of pim_system:0
endif
ifeq ($(ARGS), DPIM1)
BINARY := pim-kernel-1 # pim_system:1
START_ADDR := 0x440A00000 # must match the memory start address of pim_system:1
endif
ifeq ($(ARGS), DPIM2)
BINARY := pim-kernel-2 # pim_system:2
START_ADDR := 0x441400000 # must match the memory start address of pim_system:2
endif
ifeq ($(ARGS), DPIM3)
BINARY := pim-kernel-3 # pim_system:3
START_ADDR := 0x441E00000 # must match the memory start address of pim_system:3
endif
else
BINARY := pim-kernel # pim_system
START_ADDR := 0x440000000 # must match the memory start address of pim_system
endif

REGS := regs.h

INCLUDE := -Iinclude

ENTRY := pim_start

all: $(BINARY)

$(BINARY): $(SOURCE) $(patsubst %.c, %.h, $(SOURCE)) $(REGS)
	$(CC) \
	-g \
	$(INCLUDE) \
	-D$(ARGS) \
	$(CFLAGS) \
	-Wl,--entry=$(ENTRY),-Ttext-segment=$(START_ADDR) \
	-o $@ \
	$<

.PHONY: clean
clean:
	rm -f *.o $(wildcard pim-kernel-*)
