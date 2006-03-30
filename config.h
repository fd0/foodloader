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

#ifndef _FOODLOADER_CONFIG_H
#define _FOODLOADER_CONFIG_H

#include <avr/version.h>

/* check for avr-libc version */
#if __AVR_LIBC_VERSION__ < 10402UL
#error newer libc version (>= 1.4.2) needed!
#endif

/* check if cpu speed is defined */
#ifndef F_CPU
#error please define F_CPU!
#endif

/* check if this cpu is supported */
#if !(defined(__AVR_ATmega8__) || \
      defined(__AVR_ATmega88__) || \
      defined(__AVR_ATmega168__))
#error this cpu isn't supported yet!
#endif

/* check if the bootloader start address has been given */
#if !(defined(BOOT_SECTION_START))
#error please define BOOT_SECTION_START as the byte address of bootloader section
#endif

#define APPLICATION_SECTION_END (BOOT_SECTION_START-1)

/* cpu specific configuration registers */
#if defined(__AVR_ATmega8__)
/* {{{ */
#define _ATMEGA8

#define _TIMSK_TIMER1 TIMSK
#define _UDRIE_UART0 UDRIE
#define _TXEN_UART0 TXEN
#define _RXEN_UART0 RXEN
#define _RXCIE_UART0 RXCIE
#define _UBRRH_UART0 UBRRH
#define _UBRRL_UART0 UBRRL
#define _UCSRA_UART0 UCSRA
#define _UCSRB_UART0 UCSRB
#define _UCSRC_UART0 UCSRC
#define _UCSZ0_UART0 UCSZ0
#define _UCSZ1_UART0 UCSZ1
#define _SIG_UART_RECV_UART0 SIG_UART_RECV
#define _SIG_UART_DATA_UART0 SIG_UART_DATA
#define _UDR_UART0 UDR
#define _UDRE_UART0 UDRE
#define _RXC_UART0 RXC

/* see datasheet! */
#define _SIG_BYTE_1 0x1e
#define _SIG_BYTE_2 0x93
#define _SIG_BYTE_3 0x07

/* see avrdude configuration */
#define _AVR910_DEVCODE 0x76

/* }}} */
#elif defined(__AVR_ATmega88__)
/* {{{ */
#define _ATMEGA88

#define _TIMSK_TIMER1 TIMSK1
#define _UDRIE_UART0 UDRIE0
#define _TXEN_UART0 TXEN0
#define _RXEN_UART0 RXEN0
#define _RXCIE_UART0 RXCIE0
#define _UBRRH_UART0 UBRR0H
#define _UBRRL_UART0 UBRR0L
#define _UCSRA_UART0 UCSR0A
#define _UCSRB_UART0 UCSR0B
#define _UCSRC_UART0 UCSR0C
#define _UCSZ0_UART0 UCSZ00
#define _UCSZ1_UART0 UCSZ01
#define _SIG_UART_RECV_UART0 SIG_USART_RECV
#define _SIG_UART_DATA_UART0 SIG_USART_DATA
#define _UDR_UART0 UDR0
#define _UDRE_UART0 UDRE0
#define _RXC_UART0 RXC0

/* see datasheet! */
#define _SIG_BYTE_1 0x1e
#define _SIG_BYTE_2 0x93
#define _SIG_BYTE_3 0x0a

/* see avrdude configuration */
#define _AVR910_DEVCODE 0x33

/* }}} */
#elif defined(__AVR_ATmega168__)
/* {{{ */
#define _ATMEGA168

#define _TIMSK_TIMER1 TIMSK1
#define _UDRIE_UART0 UDRIE0
#define _TXEN_UART0 TXEN0
#define _RXEN_UART0 RXEN0
#define _RXCIE_UART0 RXCIE0
#define _UBRRH_UART0 UBRR0H
#define _UBRRL_UART0 UBRR0L
#define _UCSRA_UART0 UCSR0A
#define _UCSRB_UART0 UCSR0B
#define _UCSRC_UART0 UCSR0C
#define _UCSZ0_UART0 UCSZ00
#define _UCSZ1_UART0 UCSZ01
#define _SIG_UART_RECV_UART0 SIG_USART_RECV
#define _SIG_UART_DATA_UART0 SIG_USART_DATA
#define _UDR_UART0 UDR0
#define _UDRE_UART0 UDRE0
#define _RXC_UART0 RXC0

/* see datasheet! */
#define _SIG_BYTE_1 0x1e
#define _SIG_BYTE_2 0x94
#define _SIG_BYTE_3 0x06

/* see avrdude configuration */
#define _AVR910_DEVCODE 0x35

/* }}} */
#endif

/* debug defines */
#ifndef DEBUG
#define DEBUG 0
#endif

/* include uart support per default */
#ifndef SERIAL_UART
#define SERIAL_UART 1
#endif

/* uart configuration */
#define UART_BAUDRATE 115200


/* buffer load configuration */
#define BLOCKSIZE SPM_PAGESIZE

/* use 8 or 16 bit counter, according to the page size of the target device */
#if SPM_PAGESIZE < 256
#   define BUF_T uint8_t
#else
#   define BUF_T uint16_t
#endif

#endif /* _FOODLOADER_CONFIG_H */
