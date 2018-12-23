/* 
 * File:   debug_uart.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#include "debug_uart.h"
#include "xprintf.h"
#include <xc.h>
#include <sys/attribs.h>

void debug_buf_put(uint8_t c);
void usb_putc(uint8_t c);

// Our buffer is 2^n bytes long, and the mask is defined to match the length
#define DEBUG_PRINT_BUF_SIZE 256
#define DEBUG_PRINT_BUFFER_MASK 0xFF

typedef struct {
    uint8_t buffer[DEBUG_PRINT_BUF_SIZE];
    uint32_t head;
    uint32_t tail;
} debug_print_buffer;

debug_print_buffer debug_buf;

void init_debug_uart(void){
    debug_buf.head = 0;
    debug_buf.tail = 0;
    xdev_out(debug_buf_put);
}

void debug_buf_put(uint8_t c){
    if ( ((debug_buf.head + 1) & DEBUG_PRINT_BUFFER_MASK) == debug_buf.tail){
        return; // buffer is full
    } else {
        // critical section
        __builtin_disable_interrupts();
        debug_buf.buffer[debug_buf.head] = c;
        debug_buf.head++;
        debug_buf.head &= DEBUG_PRINT_BUFFER_MASK;
        __builtin_enable_interrupts();
    }
}

void debug_print_char(void){
    if (U1STAbits.UTXBF == 0){
        // UART is free, is there something in the buffer?
        if (debug_buf.head != debug_buf.tail){
            U1TXREG = debug_buf.buffer[debug_buf.tail];
            __builtin_disable_interrupts();
            debug_buf.tail++;
            debug_buf.tail &= DEBUG_PRINT_BUFFER_MASK;
            __builtin_enable_interrupts();
        }
    }
}

void usb_putc(uint8_t c){
    while(U1STAbits.UTXBF);
    U1TXREG = c;
}

