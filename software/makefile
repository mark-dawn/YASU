# program name
PRG=USB_saturn
# c sources files
SRC=$(wildcard *.c)
# includes files
DEPS=
DEFS=
LIBS=-Iv-usb/usbdrv -I.

# target mcu
MCU_TARGET=atmega328p
MCU_DUDE=m328p
PROG=dasa3
FUSES=-U lfuse:w:0xef:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
F_CPU=12000000UL  # in Hz
# upload file type (-O option in $(OBJCOPY)
BINTYPE=ihex
# directories
ODIR=.obj
_OBJ=$(SRC:.c=.o)
USB_DIR=v-usb/usbdrv
_OBJ += usbdrv.o usbdrvasm.o oddebug.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
#USB_OBJ=$(patsubst %,$(ODIR)/%,$(_USB_OBJ))

# optimization level
OPTIMIZE=-O2
DEBUG=#-g

####################################################
# You should not have to change anything below here.
CC=avr-gcc
# Override is only needed by avr-lib build system.
override CFLAGS= $(DEBUG) -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU) $(DEFS)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mcall-prologues
override LDFLAGS=-Wl,-Map,$(PRG).map
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

all: $(PRG).elf

# rule to make the main program
$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# rule to rebuild the objects files
$(ODIR)/%.o: %.c #$(DEPS)
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
$(ODIR)/%.o: $(USB_DIR)/%.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
$(ODIR)/%.o: $(USB_DIR)/%.S
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

# rule for lst dump
lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# rule to create the hex file for avr
hexfile: $(PRG).$(BINTYPE)

%.$(BINTYPE): %.elf
	$(OBJCOPY) -j .text -j .data -O $(BINTYPE) $< $@

#rule to program
flash: $(PRG).$(BINTYPE)
	sudo avrdude -c $(PROG) -p $(MCU_DUDE) $(FUSES) -U flash:w:$(PRG).$(BINTYPE)
# rule to create the eeprom file for avr
eeprom: $(PRG)_eeprom.$(BINTYPE)


%_eeprom.$(BINTYPE): %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O $(BINTYPE) $< $@ \
	|| { echo empty $@ not generated; exit 0; }
# rule for cleaning
.PHONY: clean
clean:
	rm -rf $(ODIR)/*.o *~ core $(PRG).elf *.$(BINTYPE) *.map *.lst

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.
FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec
dox: eps png pdf
eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf
%.eps: %.fig
	$(FIG2DEV) -L eps $< $@
%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@
%.png: %.fig
	$(FIG2DEV) -L png $< $@
