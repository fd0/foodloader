# Makefile for foodloader

# microcontroller and project specific settings (can be overridden using config.mk)
TARGET = foodloader
#F_CPU = 16000000UL
#MCU = atmega88

# include avr-generic makefile configuration
include avr.mk

ifeq ($(MCU),atmega8)
	AVRDUDE_MCU=m8
else ifeq ($(MCU),atmega88)
	AVRDUDE_MCU=m88
else ifeq ($(MCU),atmega168)
	AVRDUDE_MCU=m168
endif

# bootloader section start
# (see datasheet)
ifeq ($(MCU),atmega8)
	# atmega8 with 1024 words bootloader:
	# bootloader section starts at 0xc00 (word-address) == 0x1800 (byte-address)
	#BOOT_SECTION_START = 0x1800
	#
	# atmega8 with 512 words bootloader:
	# bootloader section starts at 0xe00 (word-address) == 0x1c00 (byte-address)
	BOOT_SECTION_START = 0x1c00
else ifeq ($(MCU),atmega88)
	# atmega88 with 1024 words bootloader:
	# bootloader section starts at 0xc00 (word-address) == 0x1800 (byte-address)
	#BOOT_SECTION_START = 0x1800
	#
	# atmega88 with 512 words bootloader:
	# bootloader section starts at 0xe00 (word-address) == 0x1c00 (byte-address)
	BOOT_SECTION_START = 0x1c00
else ifeq ($(MCU),atmega168)
	# atmega168 with 1024 words bootloader:
	# bootloader section starts at 0x1c00 (word-address) == 0x3800 (byte-address)
	#BOOT_SECTION_START = 0x3800
	#
	# atmega168 with 512 words bootloader:
	# bootloader section starts at 0x1e00 (word-address) == 0x3c00 (byte-address)
	BOOT_SECTION_START = 0x3c00
endif

LDFLAGS += -Wl,--section-start=.text=$(BOOT_SECTION_START)
CFLAGS += -DBOOT_SECTION_START=$(BOOT_SECTION_START)

OBJECTS += $(patsubst %.c,%.o,$(shell echo *.c))
HEADERS += $(shell echo *.h)
CFLAGS += -Werror
LDFLAGS += -L/usr/local/avr/avr/lib


.PHONY: all

all: $(TARGET).hex $(TARGET).lss
	@echo "==============================="
	@echo "compiled for: $(MCU)"
	@echo -n "bootloader size is: "
	@$(SIZE) -A $< | grep "\.sec1" | tr -s " " | cut -d" " -f2
	@echo "==============================="

$(TARGET): $(OBJECTS) $(TARGET).o

%.o: $(HEADERS)

.PHONY: install

install: program-isp-$(TARGET)
	@echo "don't forget: the lock-bits have been cleaned by the chip-erase!"

.PHONY: clean clean-$(TARGET)

clean: clean-$(TARGET)

clean-$(TARGET):
	rm -f $(TARGET)

