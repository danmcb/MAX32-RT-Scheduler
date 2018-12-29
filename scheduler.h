/* 
 * File:   scheduler.h
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#ifndef _SCHEDULER_H 
#define _SCHEDULER_H

#include <xc.h>

void fatal_error(int8_t * msg, int32_t i);
void init_scheduler(void);
void run_scheduler(void);
uint32_t timer_tick(void);

#endif 

