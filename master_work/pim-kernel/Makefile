CC := gcc
override CFLAGS += -Ofast -Wall -static -nostdlib

SOURCE := pim-kernel.c
BINARY := pim-kernel

REGS := regs.h

INCLUDE := -Iinclude

ENTRY := pim_start
START_ADDR := 0x440000000 # must match the memory start address of PIM

all: $(BINARY)

$(BINARY): $(SOURCE) $(patsubst %.c, %.h, $(SOURCE)) $(REGS)
	$(CC) \
	$(INCLUDE) \
	$(CFLAGS) \
	-Wl,--entry=$(ENTRY),-Ttext-segment=$(START_ADDR) \
	-o $@ \
	$<

.PHONY: clean
clean:
	rm -f *.o $(BINARY)
