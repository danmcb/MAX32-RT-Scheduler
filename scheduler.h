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

typedef struct task {
   uint32_t  period;         // Rate at which the task should tick
   uint32_t  elapsedTime;    // Time since task's last tick
   void (*TickFct)(void);       // Function to call for task's tick
} task;
                                                                                                                                                                                                                                                                   
volatile uint32_t task_scheduler_flag = 0;
uint32_t system_timer_max = 100; // we make this>0 so some LED flash is always visible
uint32_t led_counter = 0;
uint32_t tick_counter = 0;

/* TASKS */



#endif 

