/* vim:fdm=marker ts=4 et ai
 * {{{
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 }}} */

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "uart.h"

uint16_t address;                   /* start write at this address */
uint16_t data_buffer[BLOCKSIZE/2];  /* main data buffer for block-mode commands */


/* prototypes */
void (*jump_to_application)(void) = (void *)0x0000;

/* defines */
#define HIGH(x) ( (uint8_t) (x >> 8) )
#define LOW(x)  ( (uint8_t) x )

#define noinline __attribute__((noinline))


/** output one character */
static noinline void uart_putc(uint8_t data)
/*{{{*/ {

    /* put data in buffer */
    _UDR_UART0 = data;

    /* loop until data has been transmitted */
    while (!(_UCSRA_UART0 & _BV(_UDRE_UART0)));

} /* }}} */

/** output a string */
static inline void uart_puts(uint8_t buffer[])
/*{{{*/ {

    /* send everything until end of string */
    while (*buffer != 0) {
        uart_putc(*buffer);
        buffer++;
    }

} /* }}} */

/** block until one character has been read */
static noinline uint8_t uart_getc(void)
/*{{{*/ {

    /* wait if a byte has been received */
    while (!(_UCSRA_UART0 & _BV(_RXC_UART0)));

    /* return received byte */
    return _UDR_UART0;

} /* }}} */

/** init the hardware uart */
static inline void init_uart(void)
/*{{{*/ {

    /* set baud rate */
    _UBRRH_UART0 = (uint8_t)(UART_UBRR >> 8);  /* high byte */
    _UBRRL_UART0 = (uint8_t)UART_UBRR;         /* low byte */

    /* set mode */
    _UCSRC_UART0 = UART_UCSRC;

    /* enable transmitter, receiver */
    _UCSRB_UART0 = _BV(_TXEN_UART0) | _BV(_RXEN_UART0);

} /* }}} */

int main(void) {
#   if (BLOCKSIZE < 256)
        uint8_t buffer_size;
#   else
        uint16_t buffer_size;
#   endif
    uint8_t memory_type;

    init_uart();

    /* main loop */
    while (1) {
        uint8_t command;

        /* block until a command has been received */
        command = uart_getc();

        switch (command) {
            case 'P':   /* enter programming mode, respond with CR */
            case 'L':   /* leave programming mode, respond with CR */
                        uart_putc('\r');
                        break;

            case 'a':   /* report if we support address autoincrementing: yes, of course */
                        uart_putc('Y');
                        break;

            case 'A':   /* set write address start (in words), read high and low byte and respond with CR */
                        address = (uart_getc() << 8) | uart_getc();
                        uart_putc('\r');
                        break;

            /* 'c': write program memory, low byte -- NOT IMPLEMENTED */

            /* 'C': write program memory, high byte -- NOT IMPLEMENTED */

            /* 'm': issue page write -- NOT IMPLEMENTED */

            /* 'r': read lock bits -- NOT IMPLEMENTED */

            /* 'R': read program memory -- NOT IMPLEMENTED */

            /* 'd': read data (== eeprom) memory -- NOT IMPLEMENT */

            /* 'D': write data (== eeprom) memory -- NOT IMPLEMENTED */

            case 'e':   /* do a chip-erase, respond with CR afterwards */
                        /* iterate over all pages in flash, and erase every singe one of them */

                        for (address = 0; address <= APPLICATION_SECTION_END; address += SPM_PAGESIZE) {
                            boot_page_erase_safe(address);
                        }

                        uart_putc('\r');
                        break;

            /* 'l': write lock bits -- NOT IMPLEMENTED */

            /* 'F': read fuse bits -- NOT IMPLEMENTED */

            /* 'N': read high fuse bits -- NOT IMPLEMENTED */

            /* 'Q': read extended fuse bits -- NOT IMPLEMENTED */

            case 'T':   /* select device type: received device type and respond with CR */
                        /* ignore this command, only the device this bootloader
                         * is installed on can be programmed :) */
            case 'x':   /* set led, -> ignored */
            case 'y':   /* clear led, -> ignored */

                        /* discard byte and acknowledge */
                        uart_getc();
                        uart_putc('\r');
                        break;

            case 's':   /* read signature bytes: respond with the three signature bytes for this MCU */
                        uart_putc(_SIG_BYTE_1);
                        uart_putc(_SIG_BYTE_2);
                        uart_putc(_SIG_BYTE_3);
                        break;

            case 't':   /* return supported device codes (only one in this case), and terminate with a nullbyte */
                        uart_putc(_AVR910_DEVCODE);
                        uart_putc(0);
                        break;

            case 'S':   /* give software identifier, send exactly 7 chars */
                        uart_puts((uint8_t *)"FNORD!1");

                        break;

            case 'V':   /* return software version (2 byte) */
                        uart_putc('2');
                        uart_putc('3');
                        break;

            case 'p':   /* send programmer type, in this case 'S' for serial */
                        uart_putc('S');
                        break;

            case 'E':   /* exit bootloader and jump to main program */
                        jump_to_application();
                        break;

            case 'b':   /* check block support: return yes and 2 bytes block size we support */
                        uart_putc('Y');
                        uart_putc(HIGH(BLOCKSIZE));
                        uart_putc(LOW(BLOCKSIZE));
                        break;

            case 'B':   /* start block flash or eeprom load (fill mcu internal page buffer) */

                        /* first, read buffer size */
                        buffer_size = (uart_getc() << 8) | uart_getc();

                        /* check if our buffer can hold all this data */
                        if (buffer_size > BLOCKSIZE) {
                            uart_putc('?');
                            break;
                        }

                        /* then, read flash ('F') or eeprom ('E') memory type */
                        memory_type = uart_getc();

                        /* memory type is flash */
                        if (memory_type == 'F') {
                            uint16_t i;

                            /* read data into buffer */
                            for (i = 0; i < buffer_size; i++) {
                                /* read word, high byte first */
                                data_buffer[i] = (uart_getc() << 8) | uart_getc();
                            }

                            /* convert address from word to byte address */
                            address <<= 1;

                            /* iterate through the words, fill the temporary page buffer */
                            uint8_t temp_address = address;
                            boot_spm_busy_wait();

                            for (i = 0; i < BLOCKSIZE; i++) {
                                boot_page_fill(temp_address, data_buffer[i]);

                                /* increment by two, since temp_address is a byte
                                 * address, but we are writing words! */
                                temp_address += 2;
                            }

                            /* after filling the temp buffer, write the page */
                            boot_page_write_safe(address);

                            uart_putc('\r');

                        } else if (memory_type == 'E') {
                            //uart_putc('E');
                            uint8_t temp_data, i;

                            for (i = 0; i < buffer_size; i++) {
                                temp_data = uart_getc();
                                eeprom_write_byte( (uint8_t *)address, temp_data);

                                address++;
                            }

                            uart_putc('\r');

                        } else {
                            uart_putc('?');
                        }

                        break;

            case 'g':   /* start block flash or eeprom read */

                        /* first, read byte counter */
                        buffer_size = (uart_getc() << 8) | uart_getc();

                        /* then, read memory type */
                        memory_type = uart_getc();

                        /* memory type is flash */
                        if (memory_type == 'F') {

                            /* convert address from word to byte address */
                            address <<= 1;

                            /* read buffer_size words */
                            for (uint8_t i = 0; i < buffer_size; i += 2) {
                                uint16_t temp_data;

                                /* read word */
                                temp_data = pgm_read_word(address);

                                /* send data */
                                uart_putc(HIGH(data_buffer[0]));
                                uart_putc(LOW(data_buffer[0]));

                                /* increment address by 2, since it's a byte address */
                                address += 2;
                            }

                        /* if memory type is eeprom */
                        } else if (memory_type == 'E') {

                            for (uint8_t i = 0; i < buffer_size; i += 1) {
                                uint8_t temp_data;

                                /* read and send byte */
                                temp_data = eeprom_read_byte((uint8_t *)address);
                                uart_putc(temp_data);

                                address++;
                            }
                        } else {
                            uart_putc('?');
                        }

                        break;

            default:    /* default: respond with '?' */
                        uart_putc('?');
                        break;
        }

    }
}
