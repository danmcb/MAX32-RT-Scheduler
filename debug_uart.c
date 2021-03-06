/* 
 * File:   debug_uart.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#define CRITICAL_SECTION_SYNC

#include "debug_uart.h"
#include "xprintf.h"
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

volatile debug_print_buffer debug_buf;

void init_debug_uart(void){
    /* The debug print buffer. 
     * Empty when head == tail, full when tail + 1 == head
     * NOTE that head is an int that can be LARGER than the buffer size. 
     * It must ALWAYS be and'd with DEBUG_PRINT_BUFFER_MASK !
     * (This makes it possible to use __sync_fetch_and_add() to implement
     *  critical section.)
     */
    debug_buf.head = 0; 
    debug_buf.tail = 0;
    xdev_out(debug_buf_put);
}

void debug_buf_put(uint8_t c){
    if ( ((debug_buf.head + 1) & DEBUG_PRINT_BUFFER_MASK) == debug_buf.tail){
        return; // buffer is full
    } else {
        // critical section
#ifdef CRITICAL_SECTION_SYNC
        /* Critical section using LL/SC pair. This is the optimal setting. */
        volatile uint32_t * p = &debug_buf.head;
        uint32_t h = __sync_fetch_and_add(p, 1);
        debug_buf.buffer[h & DEBUG_PRINT_BUFFER_MASK] = c;
#else 
        /* no critical section for test purposes - or you can uncomment the 
         enable/disable ints for critical section at timing */
        //__builtin_disable_interrupts();
        debug_buf.buffer[(debug_buf.head & DEBUG_PRINT_BUFFER_MASK)] = c;
        debug_buf.head++;
        //__builtin_enable_interrupts();
#endif
        
    }
}

void debug_print_char(void){
    /* Print a character from the buffer, if the UART tx is free, and there is
     * one to print. Because this is only called at low priority, in the "dead" 
     * time of the main scheduler, there is no need to protect it with a 
     * critical section.
     */
    if (U1STAbits.UTXBF == 0){
        // UART is free, is there something in the buffer?
        if ( (debug_buf.head & DEBUG_PRINT_BUFFER_MASK) != debug_buf.tail){
            U1TXREG = debug_buf.buffer[debug_buf.tail];
            debug_buf.tail++;
            debug_buf.tail &= DEBUG_PRINT_BUFFER_MASK;
        }
    }
}

void usb_putc(uint8_t c){
    while(U1STAbits.UTXBF);
    U1TXREG = c;
}

