
# microcontroller and project specific settings
TARGET = foodloader
F_CPU = 16000000UL
MCU = atmega88
AVRDUDE_MCU = m88

# bootloader section start
# (see datasheet)
#
# default: atmega88 with 1024 words bootloader:
# bootloader section starts at 0xc00 (word-address) == 0x1800 (byte-address)
BOOT_SECTION_START = 0x1800
#
# atmega168 with 1024 words bootloader:
# bootloader section starts at 0x1c00 (word-address) == 0x3800 (byte-address)
#BOOT_SECTION_START = 0x3800

LDFLAGS += -Wl,--section-start=.text=$(BOOT_SECTION_START)
CFLAGS += -DBOOT_SECTION_START=$(BOOT_SECTION_START)

OBJECTS += $(patsubst %.c,%.o,$(shell echo *.c))
HEADERS += $(shell echo *.h)
CFLAGS += -Werror
LDFLAGS += -L/usr/local/avr/avr/lib

include avr.mk


.PHONY: all

all: $(TARGET).hex $(TARGET).lss

$(TARGET): $(OBJECTS) $(TARGET).o

%.o: $(HEADERS)

.PHONY: install

install: program-isp-$(TARGET)

.PHONY: clean clean-$(TARGET)

clean: clean-$(TARGET)

clean-$(TARGET):
	rm -f $(TARGET)

