/* 
 * File:   debug.h
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#ifndef _DEBUG_UART_H    
#define _DEBUG_UART_H

#include <xc.h>

void init_debug_uart(void);
void debug_print_char(void);
uint32_t get_debug_buf_head(void);

#endif // _DEBUG_UART_H 
