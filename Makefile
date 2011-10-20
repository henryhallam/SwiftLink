#
# JobyLink
# Fergus Noble, Henry Hallam (c) 2011
#

CC = sdcc

CFLAGS = --model-small --opt-code-speed

ifdef DEBUG
CFLAGS += --debug
endif

LDFLAGS_FLASH = --out-fmt-ihx --code-loc 0x1400 --code-size 0x6C00 \
	--xram-loc 0xf000 --xram-size 0xda2 --iram-size 0xff

SRC = \
	src/radio.c \
	src/main.c \
	src/hal.c \
	src/modem.c

ADB=$(SRC:.c=.adb)
ASM=$(SRC:.c=.asm)
LNK=$(SRC:.c=.lnk)
LST=$(SRC:.c=.lst)
REL=$(SRC:.c=.rel)
RST=$(SRC:.c=.rst)
SYM=$(SRC:.c=.sym)

PROGS=jobylink.hex
PCDB=$(PROGS:.hex=.cdb)
PLNK=$(PROGS:.hex=.lnk)
PMAP=$(PROGS:.hex=.map)
PMEM=$(PROGS:.hex=.mem)
PAOM=$(PROGS:.hex=)
POMF=$(PROGS:.hex=.omf)

%.rel : %.c
	$(CC) -c $(CFLAGS) -o$*.rel $<

all: $(PROGS)
#	cp $(PROGS) ../

jobylink.hex: $(REL) Makefile
	$(CC) $(LDFLAGS_FLASH) $(CFLAGS) -o jobylink.hex $(REL)

clean:
	rm -f $(ADB) $(ASM) $(LNK) $(LST) $(REL) $(RST) $(SYM)
	rm -f $(PROGS) $(PCDB) $(PLNK) $(PMAP) $(PMEM) $(PAOM) $(POMF)
#	cd ../; rm -f $(PROGS)

run: jobylink.hex
	./bootload.py /dev/tty.u* download $(PROGS)
	./bootload.py /dev/tty.u* run

